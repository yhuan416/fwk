CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

project(frameworks C)

include(GNUInstallDirs)

option(BUILD_TEST "Build Tests" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX ".debug")
endif()

# build tests
if(BUILD_TEST)
    enable_testing()
endif()

# mlib
add_subdirectory(mlib)

# elib
add_subdirectory(elib)
