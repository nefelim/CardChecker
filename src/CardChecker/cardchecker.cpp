#include <cstdio>
#include "cardchecker.h"
#include "filedevicechecker.h"
#include "progresscon.h"

void CheckDev(const std::string& path)
{
    auto progress = ProgressCon::CreateInstance();
    FileDeviceChecker fd(progress.get());
    fd.CheckDev(path);
}
