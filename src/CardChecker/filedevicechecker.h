#pragma once
#include <sys/types.h>
#include <string>
#include "iprogress.h"
#include "blockfile.h"

class FileDeviceChecker
{
public:
    FileDeviceChecker(IProgress* pProgress);
    void CheckDev(const std::string &path);

private:
    void EraseBlocks(uint64_t blockSize, uint64_t blocksCount);
    uint64_t CheckBlocks(uint64_t blockSize, uint64_t blocksCount);

private:
    IProgress* m_progress = nullptr;
    uint64_t m_blocksCount = 0; //1 Mb
    BlockFile m_file;
};
