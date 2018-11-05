#include <iostream>
#include "cardchecker.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path_to_block_device>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        CheckDev(argv[1]);
        return EXIT_SUCCESS;
    }
    catch(const std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return EXIT_FAILURE;
}
