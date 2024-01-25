#include "inputparams.h"

void InputParams::SetScanType(ScanType scanType)
{
    m_scanType = scanType;
}

void InputParams::SetPath(const std::string& path)
{
    m_path = path;
}

void InputParams::SetBufferSize(uint32_t size)
{
    m_bufferSize = size;
}

ScanType InputParams::GetScanType() const
{
    return m_scanType;
}

std::string InputParams::GetPath() const
{
    return m_path;
}

uint32_t InputParams::GetBufferSize() const
{
    return m_bufferSize;
}