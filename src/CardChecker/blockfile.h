#pragma once
#include <cstdint>
#include <string>

struct statfs64;

class BlockFile
{
public:
    BlockFile() = default;
    ~BlockFile();
    void OpenDevice(const std::string& filename, int flags);
    struct statfs64 GetDeviceInfo() const;
    uint32_t GetBlockSize();
    uint64_t GetFileSize();
    void WriteBlock(char* buffer, uint64_t blockSize, off_t offset);
    void ReadBlock(char* buffer, uint64_t blockSize, off_t offset);

private:
    int m_fd = -1;
};
