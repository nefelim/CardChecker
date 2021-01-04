#include <sstream>
#include <cstring>
#include <fcntl.h>
#include "filedevicechecker.h"

#define ALLIGNED __attribute__ ((__aligned__ (512)))

FileDeviceChecker::FileDeviceChecker(IProgress* progress)
    : m_progress(progress)
{
}

void FileDeviceChecker::EraseBlocks(uint64_t blockSize, uint64_t blocksCount)
{
    uint64_t bufferSize = blockSize * m_blocksCount;
    char buffer[bufferSize] ALLIGNED;
    std::memset(buffer, 0, bufferSize);
    uint64_t partBlocks = 0;
    off_t offset = 0;

    if (m_progress)
    {
        m_progress->SetMaxValue(blocksCount);
        m_progress->SetDescription("Erasing all blocks");
        m_progress->ShowMessage("Start erase all");
    }

    for (uint64_t block = 0; block < blocksCount; block += partBlocks)
    {
        if (m_progress)
        {
            m_progress->ShowProgress(block);
        }
        partBlocks = std::min(m_blocksCount, blocksCount - block);
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
    char buffer[blockSize] ALLIGNED;
    std::memset(buffer, 0, blockSize);
    uint64_t normalSize = 0;
    off_t offset = 0;

    if (m_progress)
    {
        m_progress->SetMaxValue(blocksCount);
        m_progress->SetDescription("Check all blocks");
        m_progress->ShowMessage("Start checking blocks");
    }

    for (uint block = 0; block < blocksCount; ++block)
    {
        if (m_progress)
        {
            m_progress->ShowProgress(block);
        }
        offset = block * blockSize;
        m_file.ReadBlock(buffer, blockSize, offset);

        uint64_t* blockNum = reinterpret_cast<uint64_t*>(buffer);
        if (*blockNum)
        {
            std::stringstream ss;
            ss << "Invalid mapping! " << block << " -> " << *blockNum;
            m_progress->ShowError(ss.str());
            continue;
        }
        *blockNum = block + 1;
        m_file.WriteBlock(buffer, blockSize, offset);
        normalSize += blockSize;
    }

    if (m_progress)
    {
        m_progress->ShowMessage("Checking of blocks completed");
    }
    return normalSize;
}

void FileDeviceChecker::CheckDev(const std::string& path)
{
    m_file.OpenDevice(path, O_RDWR | O_SYNC | O_DIRECT);

    auto blockSize = m_file.GetBlockSize();
    m_blocksCount = 1024 * 1024 / blockSize;
    auto size = m_file.GetFileSize();
    auto blocksCount = size / blockSize;

    if (m_progress)
    {
        std::stringstream ss;
        ss << "Start check device " << path << std::endl;
        ss << "Block size = " << blockSize << std::endl;
        ss << "Size = " << size << std::endl;
        ss << "Blocks count = " << blocksCount << std::endl;
        m_progress->ShowMessage(ss.str());
    }

    EraseBlocks(blockSize, blocksCount);
    auto realSize = CheckBlocks(blockSize, blocksCount);

    if (m_progress)
    {
        std::stringstream ss;
        ss << "Real size is " << realSize << "b." << std::endl;
        ss << "This is " << realSize * 100 / size << "% of the declared size";
        m_progress->ShowMessage(ss.str());
    }
}
