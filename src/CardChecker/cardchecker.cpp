#include <cstdio>
#include "cardchecker.h"
#include "iinputparams.h"
#include "filedevicechecker.h"

void CheckDev(const IInputParams& inputParams, IProgress* progress)
{
    FileDeviceChecker fd(inputParams, progress);
    fd.CheckDev();
}
