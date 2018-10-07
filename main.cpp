#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>

uint32_t GetDeviceBlockSize(int fd)
{
    struct stat s = {0};
    if (fstat(fd, &s))
    {
        std::stringstream ss;
        ss << "Can't get fstat for device. Error = " << errno << std::endl;
        throw std::runtime_error(ss.str());
    }
    return s.st_blksize;
}

uint64_t GetFileSize(int fd)
{
    off_t offset = lseek64(fd, 0, SEEK_END);
    if (offset == -1)
    {
        std::stringstream ss;
        ss << "Can't get file size. Error = " << errno << std::endl;
        throw std::runtime_error(ss.str());
    }
    return offset;
}

void WriteBlock(int fd, char* buffer, uint32_t blockSize, uint64_t offset)
{
    auto res = pwrite64(fd, buffer, blockSize, offset);
    if (res == -1)
    {
        std::stringstream ss;
        ss << "Can't write to offset " << offset << ". Error = " << errno << std::endl;
        throw std::runtime_error(ss.str());
    }
}

void EraseBlocks(int fd, uint32_t blockSize, uint64_t blocksCount)
{
    char buffer[blockSize] __attribute__ ((__aligned__ (512))) = {0};
    for (uint64_t block = 0; block < blocksCount; ++block)
    {
        WriteBlock(fd, buffer, blockSize, block * blockSize);
    }
}

uint64_t CheckBlocks(int fd, uint32_t blockSize, uint64_t blocksCount)
{
    char buffer[blockSize] __attribute__ ((__aligned__ (512))) = {0};
    off_t offset = 0;
    uint64_t normalSize = 0;

    offset = lseek64(fd, 0, SEEK_SET);
    if (offset == -1)
    {
        std::stringstream ss;
        ss << "Can't seek to offset " << offset << ". Error = " << errno << std::endl;
        throw std::runtime_error(ss.str());
    }

    for (uint64_t block = 0; block < blocksCount; ++block)
    {
        offset = block * blockSize;
        auto readedBytes = pread(fd, buffer, blockSize, offset);
        if (readedBytes == -1)
        {
            std::stringstream ss;
            ss << "Can't read from offset " << offset << ". Error = " << errno << std::endl;
            throw std::runtime_error(ss.str());
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
        ss << "Can't open file " << path << " error = " << errno << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << "Start check device " << path << std::endl;
    auto blockSize = GetDeviceBlockSize(fd);
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
    }
    catch(const std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
