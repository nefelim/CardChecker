#pragma once

#include "iinputparams.h"

class InputParams : public IInputParams
{
public:
    void SetScanType(ScanType scanType) override;
    void SetPath(const std::string& path) override;
    void SetBufferSize(uint32_t size) override;

    ScanType GetScanType() const override;
    std::string GetPath() const override;
    uint32_t GetBufferSize() const override;

private:
    ScanType m_scanType = ScanType::None;
    std::string m_path;
    uint32_t m_bufferSize = 1024 * 1024; // 1Mb
};