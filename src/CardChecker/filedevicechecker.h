#pragma once
#include <sys/types.h>
#include <string>
#include "iprogress.h"
#include "iinputparams.h"
#include "blockfile.h"

class FileDeviceChecker
{
public:
    FileDeviceChecker(const IInputParams& inputParams, IProgress* pProgress);
    void CheckDev();

private:
    void EraseBlocks(uint64_t blockSize, uint64_t blocksCount);
    uint64_t CheckBlocks(uint64_t blockSize, uint64_t blocksCount);
    static std::string FSTypeToStr(const __fsword_t& fsType);
    static std::string FSIDToStr(const __fsid_t& s);
    static std::string DeviceInfoToStr(const struct statfs64& s);

private:
    IProgress* m_progress = nullptr;
    const IInputParams& m_inputParams;
    uint64_t m_blocksPerBufferCount = 0;
    BlockFile m_file;
};
