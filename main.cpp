#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <system_error>

#define ALLIGNED __attribute__ ((__aligned__ (512)))
uint64_t g_blocksCount = 0; //1 Mb

void ShowProgress(uint64_t currentValue, uint64_t maxValue, uint64_t& showedPercents)
{
    uint64_t currentPercents = currentValue * 100 / (maxValue - 1);
    if (currentPercents != showedPercents)
    {
        std::cout << currentPercents << "%" << std::endl;
        showedPercents = currentPercents;
    }
}

uint32_t GetDeviceBlockSize(int fd)
{
    struct stat s = {};
    if (fstat(fd, &s))
    {
        throw std::system_error(errno, std::system_category(), "Can't get fstat for device.");
    }
    return static_cast<uint32_t>(s.st_blksize);
}

uint64_t GetFileSize(int fd)
{
    off_t offset = lseek64(fd, 0, SEEK_END);
    if (offset == -1)
    {
        throw std::system_error(errno, std::system_category(), "Can't get file size.");
    }
    return static_cast<uint64_t>(offset);
}

void WriteBlock(int fd, char* buffer, uint64_t blockSize, off_t offset)
{
    auto res = pwrite64(fd, buffer, blockSize, offset);
    if (res == -1)
    {
        std::stringstream ss;
        ss << "Can't write to offset " << offset << " block size " << blockSize;
        throw std::system_error(errno, std::system_category(), ss.str());
    }
}

void EraseBlocks(int fd, uint64_t blockSize, uint64_t blocksCount)
{
    uint64_t bufferSize = blockSize * g_blocksCount;
    char buffer[bufferSize] ALLIGNED;
    std::memset(buffer, 0, bufferSize);
    uint64_t showedPercents = 0;
    uint64_t partBlocks = 0;
    off_t offset = 0;
    for (uint64_t block = 0; block < blocksCount; block += partBlocks)
    {
        ShowProgress(block, blocksCount, showedPercents);
        partBlocks = std::min(g_blocksCount, blocksCount - block);
        bufferSize = partBlocks * blockSize;
        WriteBlock(fd, buffer, bufferSize, offset);
        offset += bufferSize;
    }
}

uint64_t CheckBlocks(int fd, uint64_t blockSize, uint64_t blocksCount)
{
    char buffer[blockSize] ALLIGNED;
    std::memset(buffer, 0, blockSize);
    off_t offset = 0;
    uint64_t normalSize = 0;

    offset = lseek64(fd, 0, SEEK_SET);
    if (offset == -1)
    {
        std::stringstream ss;
        ss << "Can't seek to offset " << offset;
        throw std::system_error(errno, std::system_category(), ss.str());
    }

    uint64_t showedPercents = 0;

    for (uint block = 0; block < blocksCount; ++block)
    {
        ShowProgress(block, blocksCount, showedPercents);
        offset = block * blockSize;
        auto readedBytes = pread(fd, buffer, blockSize, offset);
        if (readedBytes == -1)
        {
            std::stringstream ss;
            ss << "Can't read from offset " << offset;
            throw std::system_error(errno, std::system_category(), ss.str());
        }

        auto& blockNum = *reinterpret_cast<uint64_t*>(buffer);
        if (blockNum)
        {
            std::cout << "Invalid mapping! " << block << " -> " << blockNum << std::endl;
            continue;
        }
        blockNum = block + 1;
        WriteBlock(fd, buffer, blockSize, offset);
        normalSize += blockSize;        
    }
    return normalSize;
}

void CheckDev(const std::string& path)
{
    int fd = open(path.c_str(), O_RDWR | O_SYNC | O_DIRECT);
    if (fd == -1)
    {
        std::stringstream ss;
        ss << "Can't open file " << path;
        throw std::system_error(errno, std::system_category(), ss.str());
    }

    std::cout << "Start check device " << path << std::endl;
    auto blockSize = GetDeviceBlockSize(fd);
    g_blocksCount = 1024 * 1024 / blockSize;
    std::cout << "Block size = " << blockSize << std::endl;
    auto size = GetFileSize(fd);
    std::cout << "Size = " << size << std::endl;
    auto blocksCount = size / blockSize;
    std::cout << "Blocks count = " << blocksCount << std::endl;

    std::cout << "Start erase all" << std::endl;
    EraseBlocks(fd, blockSize, blocksCount);
    std::cout << "Erasing completed" << std::endl;

    std::cout << "Start checking blocks" << std::endl;
    auto normalSize = CheckBlocks(fd, blockSize, blocksCount);
    std::cout << "Checking completed" << std::endl;
    std::cout << "Normal size = " << normalSize << std::endl;
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <pathname>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        CheckDev(argv[1]);
        return EXIT_SUCCESS;
    }
    catch(const std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return EXIT_FAILURE;
}
