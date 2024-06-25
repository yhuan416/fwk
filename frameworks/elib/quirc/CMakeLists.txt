cmake_minimum_required (VERSION 3.2)

project(quirc C)

set(quirc_VERSION 1.2)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

# 获取src目录下的所有文件
file(GLOB srcs ${CMAKE_CURRENT_SOURCE_DIR}/quirc-1.2/lib/*.c)

if(BUILD_SHARED_LIBS)
    add_library(quirc SHARED ${srcs})

    set_target_properties(quirc PROPERTIES
        SOVERSION ${quirc_VERSION})

    target_include_directories(quirc INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/quirc-1.2/lib>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(quirc PRIVATE m)

    install(TARGETS quirc EXPORT quirc-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(BUILD_STATIC_LIBS)
    add_library(quirc-static STATIC ${srcs})

    target_include_directories(quirc-static INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/quirc-1.2/lib>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    set_target_properties(quirc-static PROPERTIES 
        OUTPUT_NAME quirc)

    target_link_libraries(quirc-static PRIVATE m)

    target_compile_options(quirc-static PRIVATE -fPIC)

    install(TARGETS quirc-static EXPORT quirc-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

install(EXPORT quirc-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quirc
    NAMESPACE quirc::
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/quirc-1.2/lib/quirc.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
