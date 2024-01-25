#pragma once
#include "progressbase.h"
#include <memory>

class ProgressCon : private ProgressBase<uint64_t>
{
private:
    using Base = ProgressBase<uint64_t>;
    using ProgressPtr = std::unique_ptr<IProgress>;

public:
    static ProgressPtr CreateInstance();

private:
    ProgressCon() = default;
    void SetDescription(const std::string& description) override;
    void ShowMessage(const std::string& message) const override;
    void ShowError(const std::string& error) const override;
    void DrawProgress(int64_t currentPercents) const override;
};
