#pragma once
#include <cstdint>
#include <string>

enum class ScanType
{
    None = 0,
    Quick,
    Deep
};

class IInputParams
{
public:
    virtual ~IInputParams() {}

    virtual void SetScanType(ScanType scanType) = 0;
    virtual void SetPath(const std::string& description) = 0;
    virtual void SetBufferSize(uint32_t size) = 0;

    virtual ScanType GetScanType() const = 0;
    virtual std::string GetPath() const = 0;
    virtual uint32_t GetBufferSize() const = 0;

};
