cmake_minimum_required (VERSION 3.2)

project(easylogger C)

set(easylogger_VERSION 2.2.99)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

# easylogger srcs
file(GLOB easylogger_src ${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger/easylogger/src/*.c)
set(easylogger_plugin_file ${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger/easylogger/plugins/file/elog_file.c)

# port srcs
file(GLOB port_src ${CMAKE_CURRENT_SOURCE_DIR}/port/*.c)

if(BUILD_SHARED_LIBS)
    add_library(easylogger SHARED
        ${port_src}
        ${easylogger_src}
        ${easylogger_plugin_file}
    )

    set_target_properties(easylogger PROPERTIES
        SOVERSION ${easylogger_VERSION})
    
    target_include_directories(easylogger PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/port>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger/easylogger/inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger/easylogger/plugins/file>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(easylogger PRIVATE pthread osal)

    install(TARGETS easylogger EXPORT easylogger-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(BUILD_STATIC_LIBS)
    add_library(easylogger-static STATIC
        ${port_src}
        ${easylogger_src}
        ${easylogger_plugin_file}
    )

    target_include_directories(easylogger-static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/port>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger/easylogger/inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger/easylogger/plugins/file>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(easylogger-static PRIVATE pthread osal)

    target_compile_options(easylogger-static PRIVATE -fPIC)

    set_target_properties(easylogger-static PROPERTIES
        OUTPUT_NAME easylogger)

    install(TARGETS easylogger-static EXPORT easylogger-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

install(EXPORT easylogger-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easylogger
    NAMESPACE easylogger::
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger/easylogger/inc/elog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/port/elog_cfg.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
