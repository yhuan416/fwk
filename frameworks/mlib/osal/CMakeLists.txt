cmake_minimum_required(VERSION 3.5)

# 获取src目录下的所有文件
file(GLOB srcs src/*.c)

file(GLOB headers src/*.h)

if (ESP_PLATFORM)
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "src"
                    REQUIRES freertos log)
target_compile_options(${COMPONENT_LIB} PRIVATE -Dno_use_lock_free_queue)
else ()

project(osal C)

add_library(osal STATIC ${srcs})

target_include_directories(osal PUBLIC 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(osal PRIVATE pthread rt)

target_compile_options(osal PRIVATE -fPIC)

# target_compile_options(osal PRIVATE -Dno_use_lock_free_queue)

install(TARGETS osal EXPORT osal-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT osal-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osal
    NAMESPACE osal::
)

install(FILES ${headers}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(BUILD_TEST)
    add_subdirectory(testsuites)
endif()

endif()
