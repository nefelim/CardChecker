cmake_minimum_required(VERSION 3.5)
#common properties
include(cmake/config.cmake)

# Project name
project(CardChecker)
set(PACKAGE_NAME CardChecker)

#build
add_subdirectories(
    CardCheckerCli
    CardChecker
    )
