
project(netx C)

set(netx_VERSION 0.1.0)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

if(BUILD_SHARED_LIBS)
    add_library(netx SHARED
        netx.c
        netx.h)

    set_target_properties(netx PROPERTIES
        SOVERSION ${netx_VERSION})
    
    target_include_directories(netx INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

    target_compile_definitions(netx PUBLIC _epoll _getifaddrs)
endif()

if(BUILD_STATIC_LIBS)
    add_library(netx-static STATIC
        netx.c
        netx.h)

    target_include_directories(netx-static INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

    target_compile_definitions(netx-static PUBLIC _epoll _getifaddrs)

    set_target_properties(netx-static PROPERTIES
            OUTPUT_NAME netx)
endif()
