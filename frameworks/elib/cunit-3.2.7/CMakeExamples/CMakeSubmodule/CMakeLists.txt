project(CUnitExampleSubmodule C)
cmake_minimum_required(VERSION 3.5)

add_subdirectory(cunit-x.y.z/CUnit)

add_executable(test-program test-program.c)
target_link_libraries(test-program PUBLIC cunit)