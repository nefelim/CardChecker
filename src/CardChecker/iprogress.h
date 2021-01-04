#pragma once
#include <string>

class IProgress
{
public:
    virtual ~IProgress() {}
    virtual void SetMaxValue(uint64_t maxValue) = 0;
    virtual void SetDescription(const std::string& description) = 0;

    virtual void ShowMessage(const std::string& message) const = 0;
    virtual void ShowProgress(uint64_t currentValue) const = 0;
    virtual void ShowError(const std::string& error) const = 0;
    virtual void DrawProgress(int64_t currentPercents) const = 0;
};
