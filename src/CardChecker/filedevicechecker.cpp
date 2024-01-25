#include <sstream>
#include <cstring>
#include <fcntl.h>
#include <sys/statfs.h>
#include "filedevicechecker.h"

#define ALLIGNED __attribute__((__aligned__(512)))

FileDeviceChecker::FileDeviceChecker(const IInputParams& inputParams, IProgress* progress)
    : m_progress(progress), m_inputParams(inputParams)
{
}

void FileDeviceChecker::EraseBlocks(uint64_t blockSize, uint64_t blocksCount)
{
    uint64_t bufferSize = blockSize * m_blocksPerBufferCount;
    char buffer[bufferSize] ALLIGNED;
    std::memset(buffer, 0, bufferSize);
    uint64_t partBlocks = 0;
    off_t offset = 0;

    if (m_progress)
    {
        m_progress->SetMaxValue(blocksCount);
        m_progress->SetDescription("Erasing all blocks");
        m_progress->ShowMessage("Erasing the device starts");
    }

    for (uint64_t block = 0; block < blocksCount; block += partBlocks)
    {
        if (m_progress)
        {
            m_progress->ShowProgress(block);
        }
        partBlocks = std::min(m_blocksPerBufferCount, blocksCount - block);
        bufferSize = partBlocks * blockSize;
        m_file.WriteBlock(buffer, bufferSize, offset);
        offset += bufferSize;
    }

    if (m_progress)
    {
        m_progress->ShowMessage("Erasing completed");
    }
}

uint64_t FileDeviceChecker::CheckBlocks(uint64_t blockSize, uint64_t blocksCount)
{
    uint64_t bufferSize = blockSize * m_blocksPerBufferCount;
    char buffer[bufferSize] ALLIGNED;
    std::memset(buffer, 0, bufferSize);
    uint64_t partBlocks = 0;
    uint64_t normalSize = 0;

    if (m_progress)
    {
        m_progress->SetMaxValue(blocksCount);
        m_progress->SetDescription("Check all blocks");
        m_progress->ShowMessage("Checking the device starts");
    }

    for (uint64_t block = 0; block < blocksCount; block += partBlocks)
    {
        if (m_progress)
        {
            m_progress->ShowProgress(block);
        }
        partBlocks = std::min(m_blocksPerBufferCount, blocksCount - block);
        bufferSize = partBlocks * blockSize;
        auto offset = block * blockSize;
        m_file.ReadBlock(buffer, bufferSize, offset);
        for (uint32_t localBlock = 0; localBlock < partBlocks; ++localBlock)
        {
            uint64_t* blockNum = reinterpret_cast<uint64_t*>(buffer + localBlock * blockSize);
            if (*blockNum)
            {
                std::stringstream ss;
                ss << "Invalid mapping! " << block + localBlock << " -> " << *blockNum;
                m_progress->ShowError(ss.str());
                continue;
            }
            *blockNum = block + localBlock + 1;
        }
        m_file.WriteBlock(buffer, bufferSize, offset);
        normalSize += bufferSize;
    }

    if (m_progress)
    {
        m_progress->ShowMessage("Checking of blocks completed");
    }
    return normalSize;
}

std::string FileDeviceChecker::FSTypeToStr(const __fsword_t& fsType)
{
    std::stringstream ss;
    switch (fsType)
    {
    default: ss << "0x" << std::hex << fsType;
    }
    return ss.str();
}

std::string FileDeviceChecker::FSIDToStr(const __fsid_t& s)
{
    std::stringstream ss;
    ss << "0x" << std::hex << s.__val[0] << s.__val[1];
    return ss.str();
}

std::string FileDeviceChecker::DeviceInfoToStr(const struct statfs64& s)
{
    std::stringstream ss;
    ss << "Device Info: " << std::endl;
    ss << "Type of file system: " << s.f_type << std::endl;
    ss << "Optimal transfer block size: " << s.f_bsize << std::endl;
    ss << "Total data blocks in file system: " << s.f_blocks << std::endl;
    ss << "Free blocks in fs: " << s.f_bfree << std::endl;
    ss << "Free blocks available to unprivileged user: " << s.f_bavail << std::endl;
    ss << "Total file nodes in file system: " << s.f_files << std::endl;
    ss << "Free file nodes in fs: " << s.f_ffree << std::endl;
    ss << "File system id: " << FSIDToStr(s.f_fsid) << std::endl;
    ss << "Maximum length of filenames: " << s.f_namelen << std::endl;
    ss << "Fragment size: " << s.f_frsize << std::endl;
    return ss.str();
}

void FileDeviceChecker::CheckDev()
{
    auto path = m_inputParams.GetPath();
    m_file.OpenDevice(path, O_RDWR | O_SYNC | O_DIRECT);

    const auto blockSize = m_file.GetBlockSize();
    m_blocksPerBufferCount = m_inputParams.GetBufferSize() / blockSize;
    auto deviceSize = m_file.GetFileSize();
    auto blocksPerDevice = deviceSize / blockSize;

    if (m_progress)
    {
        std::stringstream ss;
        ss << "Start check device " << path << std::endl;
        ss << "File Info:" << std::endl;
        ss << "Block size: " << blockSize << std::endl;
        ss << "Device size: " << deviceSize << std::endl;
        ss << "Blocks count:  " << blocksPerDevice << std::endl;

        auto deviceInfo = m_file.GetDeviceInfo();
        ss << DeviceInfoToStr(deviceInfo);
        m_progress->ShowMessage(ss.str());
    }

    EraseBlocks(blockSize, blocksPerDevice);
    auto realSize = CheckBlocks(blockSize, blocksPerDevice);

    if (m_progress)
    {
        std::stringstream ss;
        ss << "Real size is " << realSize << " b." << std::endl;
        ss << "This is " << realSize * 100 / deviceSize << "% of the declared size.";
        m_progress->ShowMessage(ss.str());
    }
}
