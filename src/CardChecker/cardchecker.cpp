#include <cstdio>
#include "cardchecker.h"
#include "filedevicechecker.h"

void CheckDev(const std::string& path, IProgress* progress)
{
    FileDeviceChecker fd(progress);
    fd.CheckDev(path);
}
