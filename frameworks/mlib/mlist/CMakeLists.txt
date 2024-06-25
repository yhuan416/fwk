cmake_minimum_required(VERSION 3.2)

project(mlist C)

add_library(mlist INTERFACE)

target_include_directories(mlist INTERFACE 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mlist>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

install(TARGETS mlist EXPORT mlist-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT mlist-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mlist
    NAMESPACE mlist::
)

install(FILES mlist.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mlist
)
