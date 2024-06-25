cmake_minimum_required(VERSION 3.5)

project(mmem_debug C)

option(BUILD_TEST "Build mmem_debug test" OFF)

if (BUILD_TEST)
    message(STATUS "mmem_debug: Build test: ${BUILD_TEST}")
endif ()

file(GLOB MMEM_DEBUG_SRC src/*.c)
file(GLOB MMEM_DEBUG_HDRS include/*.h)

file(GLOB MMEM_DEBUG_ADAPTER_SRC adapter/*.c)
file(GLOB MMEM_DEBUG_ADAPTER_HDRS adapter/*.h)

# mmem_debug_adapter
add_subdirectory(adapter)

add_library(mmem_debug STATIC ${MMEM_DEBUG_SRC} ${MMEM_DEBUG_ADAPTER_SRC})

target_include_directories(mmem_debug PUBLIC 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/adapter>
)

target_link_libraries(mmem_debug PRIVATE mlist)

target_compile_options(mmem_debug PRIVATE -fPIC)

install(TARGETS mmem_debug EXPORT mmem_debug-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT mmem_debug-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mmem_debug
    NAMESPACE mmem_debug::
)

install(FILES ${MMEM_DEBUG_HDRS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# test build
if (BUILD_TEST)
    add_subdirectory(test)
endif ()
