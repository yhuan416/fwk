cmake_minimum_required(VERSION 3.2)

if (ESP_PLATFORM)
idf_component_register(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
else ()

project(mlist C)

add_library(mlist INTERFACE)

target_include_directories(mlist INTERFACE 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

endif ()
