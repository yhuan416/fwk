
project(netx C)

set(netx_VERSION 0.1.0)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

option(BUILD_EXAMPLES "Build examples code" OFF)

if(BUILD_SHARED_LIBS)
    add_library(netx SHARED
        netx.c
        netx.h)

    set_target_properties(netx PROPERTIES
        SOVERSION ${netx_VERSION})
    
    target_include_directories(netx INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

    target_compile_definitions(netx PUBLIC _epoll _getifaddrs)

    install(TARGETS netx EXPORT netx-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(BUILD_STATIC_LIBS)
    add_library(netx-static STATIC
        netx.c
        netx.h)

    target_include_directories(netx-static INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

    target_compile_definitions(netx-static PUBLIC _epoll _getifaddrs)

    set_target_properties(netx-static PROPERTIES
        OUTPUT_NAME netx)

    install(TARGETS netx-static EXPORT netx-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

install(EXPORT netx-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/netx
    NAMESPACE netx::
)

install(FILES netx.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
