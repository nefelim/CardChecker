#include <sys/stat.h>
#include <sys/statfs.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>
#include <sstream>

#include "blockfile.h"

BlockFile::~BlockFile()
{
    ::close(m_fd);
}

void BlockFile::OpenDevice(const std::string& path, int flags)
{
    m_fd = ::open(path.c_str(), flags);
    if (m_fd == -1)
    {
        std::stringstream ss;
        ss << "Can't open file " << path;
        throw std::system_error(errno, std::system_category(), ss.str());
    }
}

struct statfs64 BlockFile::GetDeviceInfo() const
{
    struct statfs64 s = {};
    if (::fstatfs64(m_fd, &s))
    {
        throw std::system_error(errno, std::system_category(), "Can't get statfs64 for device.");
    }
    return s;
}

uint32_t BlockFile::GetBlockSize()
{
    struct stat64 s = {};
    if (::fstat64(m_fd, &s))
    {
        throw std::system_error(errno, std::system_category(), "Can't get fstat for device.");
    }
    return static_cast<uint32_t>(s.st_blksize);
}

uint64_t BlockFile::GetFileSize()
{
    off_t offset = ::lseek64(m_fd, 0, SEEK_END);
    if (offset == -1)
    {
        throw std::system_error(errno, std::system_category(), "Can't get file size.");
    }
    return static_cast<uint64_t>(offset);
}

void BlockFile::WriteBlock(char* buffer, uint64_t blockSize, off_t offset)
{
    auto res = ::pwrite64(m_fd, buffer, blockSize, offset);
    if (res == -1)
    {
        std::stringstream ss;
        ss << "Can't write to offset " << offset << " block size " << blockSize;
        throw std::system_error(errno, std::system_category(), ss.str());
    }
}

void BlockFile::ReadBlock(char* buffer, uint64_t blockSize, off_t offset)
{
    auto readedBytes = pread(m_fd, buffer, blockSize, offset);
    if (readedBytes == -1)
    {
        std::stringstream ss;
        ss << "Can't read from offset " << offset;
        throw std::system_error(errno, std::system_category(), ss.str());
    }
}
