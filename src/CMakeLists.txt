cmake_minimum_required(VERSION 2.8)
#common properties
include(cmake/config.cmake)

#build
add_subdirectories(
    CardCheckerCli
    CardChecker
    )
