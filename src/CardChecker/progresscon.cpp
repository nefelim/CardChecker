#include "progresscon.h"
#include <iostream>

ProgressCon::ProgressPtr ProgressCon::CreateInstance()
{
    return ProgressCon::ProgressPtr(new ProgressCon());
}

void ProgressCon::SetDescription(const std::string& description)
{
    std::cout << description << std::endl;
}

void ProgressCon::ShowMessage(const std::string& message) const
{
    std::cout << message << std::endl;
}

void ProgressCon::ShowError(const std::string& error) const
{
    std::cerr << error << std::endl;
}

void ProgressCon::DrawProgress(int64_t currentPercents) const
{
    std::cout << currentPercents << "%" << std::endl;
}
