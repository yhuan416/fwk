cmake_minimum_required(VERSION 3.5)

project(mmem_debug_tests C)

set(common_libs cunit mmem_debug)

# 获取test目录下的所有文件
file(GLOB TEST_FILES tests/*.c)

# 为每个测试文件创建一个可执行文件
foreach(TEST_FILE ${TEST_FILES})
    message(STATUS "TEST_FILE: ${TEST_FILE}")

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    message(STATUS "TEST_NAME: ${TEST_NAME}")

    # add executable
    add_executable(${TEST_NAME} ${TEST_FILE})

    # add include path
    target_include_directories(${TEST_NAME} PRIVATE include)

    # add link library
    target_link_libraries(${TEST_NAME} ${common_libs})

    # add compile definition
    target_compile_definitions(${TEST_NAME} PRIVATE __mmem_debug_enbale=1)

    # add test, if use valgrind, add valgrind command
    if (use_valgrind)
        add_test(NAME ${TEST_NAME} 
            COMMAND valgrind 
                    --leak-check=full 
                    --show-reachable=yes 
                    --trace-children=yes 
                    --error-exitcode=1 
                    --suppressions=${CMAKE_CURRENT_SOURCE_DIR}/valgrind/double_free.supp
                    $<TARGET_FILE:${TEST_NAME}>)
    else ()
        add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    endif ()
endforeach()
