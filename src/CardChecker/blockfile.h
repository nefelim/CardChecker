#pragma once
#include <string>

class BlockFile
{
public:
    BlockFile() = default;
    ~BlockFile();
    void OpenDevice(const std::string& filename, int flags);
    uint32_t GetBlockSize();
    uint64_t GetFileSize();
    void WriteBlock(char *buffer, uint64_t blockSize, off_t offset);
    void ReadBlock(char *buffer, uint64_t blockSize, off_t offset);
private:
    int m_fd = -1;
};
