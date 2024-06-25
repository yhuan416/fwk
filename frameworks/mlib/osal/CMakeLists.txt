cmake_minimum_required(VERSION 3.5)

# 获取src目录下的所有文件
file(GLOB_RECURSE srcs src/*.c)

if (ESP_PLATFORM)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "src"
                    REQUIRES freertos log)

target_compile_options(${COMPONENT_LIB} PRIVATE -Dno_use_lock_free_queue)

else ()

project(osal C)

add_library(osal STATIC ${srcs})

target_include_directories(osal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(osal PRIVATE pthread rt)

target_compile_options(osal PRIVATE -fPIC)

# target_compile_options(osal PRIVATE -Dno_use_lock_free_queue)

if(BUILD_TEST)
    add_subdirectory(testsuites)
endif()

endif()
