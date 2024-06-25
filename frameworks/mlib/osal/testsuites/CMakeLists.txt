cmake_minimum_required(VERSION 3.5)

project(osal_test C)

set(common_libs cunit osal pthread)

# 获取目录下的所有文件
file(GLOB TEST_FILES 
    mem/*.c
    misc/*.c
    task/*.c
    sem/*.c
    mq/*.c
)

add_executable(${PROJECT_NAME} ${TEST_FILES} osal_test.c)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} ${common_libs})

add_test(NAME ${PROJECT_NAME} COMMAND valgrind --leak-check=full --show-reachable=yes --trace-children=yes --error-exitcode=1 $<TARGET_FILE:${PROJECT_NAME}>)
