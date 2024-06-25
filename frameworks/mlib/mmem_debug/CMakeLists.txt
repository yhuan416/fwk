cmake_minimum_required(VERSION 3.5)

project(mmem_debug C)

option(BUILD_TEST "Build mmem_debug test" OFF)

if (BUILD_TEST)
    message(STATUS "mmem_debug: Build test: ${BUILD_TEST}")
endif ()

file(GLOB MMEM_DEBUG_SRC src/*.c)

# mmem_debug_adapter
add_subdirectory(adapter)

add_library(mmem_debug STATIC ${MMEM_DEBUG_SRC})

target_include_directories(mmem_debug PUBLIC include)

target_link_libraries(mmem_debug PRIVATE mmem_debug_adapter mlist)

# test build
if (BUILD_TEST)
    add_subdirectory(test)
endif ()
