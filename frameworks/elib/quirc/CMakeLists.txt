cmake_minimum_required (VERSION 3.2)

project(quirc C)

set(quirc_VERSION 1.2)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

# 获取src目录下的所有文件
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/quirc-1.2/lib/*.c)

if(BUILD_SHARED_LIBS)
    add_library(quirc SHARED ${srcs})

    set_target_properties(quirc PROPERTIES
        SOVERSION ${quirc_VERSION})

    target_include_directories(quirc INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/quirc-1.2/lib>
    )

    target_link_libraries(quirc PRIVATE m)
endif()

if(BUILD_STATIC_LIBS)
    add_library(quirc-static STATIC ${srcs})

    target_include_directories(quirc-static INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/quirc-1.2/lib>
    )

    set_target_properties(quirc-static PROPERTIES 
        OUTPUT_NAME quirc)

    target_link_libraries(quirc-static PRIVATE m)
endif()
