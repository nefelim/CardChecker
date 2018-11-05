# Project name
set(PACKAGE_NAME CardChecker)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif ()

set(ROOT "${CMAKE_SOURCE_DIR}/..")
set(OBJ_DIR "${ROOT}/obj/${CMAKE_BUILD_TYPE}")
set(LIB_DIR "${ROOT}/lib/${CMAKE_BUILD_TYPE}")
set(BIN_DIR "${ROOT}/bin/${CMAKE_BUILD_TYPE}")

# Installation dir (if using cpack for generating packages)
set(BIN_INSTALL_DIR opt/.${PACKAGE_NAME})
set(LIBS_INSTALL_DIR ${BIN_INSTALL_DIR}/libs/)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIB_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIB_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}")

macro(make_project_)
    if (NOT DEFINED PROJECT)
        get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    endif ()

    project(${PROJECT})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14 -fstrict-aliasing -pedantic-errors -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-vla") #Disable error: ISO C++ forbids variable length array ‘buffer’ [-Wvla]

    if (NOT DEFINED HEADERS)
        file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    endif ()

    if (NOT DEFINED SOURCES)
        file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    endif ()

    source_group("Header Files" FILES ${HEADERS})
    source_group("Source Files" FILES ${SOURCES})
endmacro()

macro(make_executable)
    make_project_()
    add_executable(${PROJECT} ${HEADERS} ${SOURCES})
endmacro()

macro(make_library)
    make_project_()
    add_library(${PROJECT} STATIC ${HEADERS} ${SOURCES})
    target_include_directories(${PROJECT} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

    if (NOT SOURCES)
        set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)
    endif ()
endmacro()

function(add_subdirectories)
    if (ARGV)
        set(DIRECTORIES ${ARGV})
    else ()
        set(DIRECTORIES )
        file(GLOB CHILDREN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

        foreach (CHILD ${CHILDREN})
            if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CHILD})
                set(DIRECTORIES ${DIRECTORIES} ${CHILD})
            endif ()
        endforeach ()
    endif ()

    foreach (DIR ${DIRECTORIES})
        add_subdirectory(${DIR})
    endforeach ()
endfunction()

