#include <iostream>
#include "progresscon.h"
#include "inputparams.h"
#include "cardchecker.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_block_device>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        auto progress = ProgressCon::CreateInstance();
        InputParams inputParams;
        inputParams.SetPath(argv[1]);
        CheckDev(inputParams, progress.get());
        return EXIT_SUCCESS;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return EXIT_FAILURE;
}
