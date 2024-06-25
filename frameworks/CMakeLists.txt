CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

project(frameworks C)

# build tests
if(BUILD_TEST)
    enable_testing()
endif()

# mlib
add_subdirectory(mlib)

# elib
add_subdirectory(elib)
