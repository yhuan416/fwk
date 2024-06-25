project(CUnitExamplePackage C)
cmake_minimum_required(VERSION 3.5)

# uncomment to test pre-building cunit

#set(EXT_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext/cunit/install)
#set(EXT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext/cunit/build)
#
#file(MAKE_DIRECTORY ${EXT_INSTALL_DIR})
#file(MAKE_DIRECTORY ${EXT_BUILD_DIR})
#execute_process(
#  COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR} ${CMAKE_SOURCE_DIR}/../../
#  WORKING_DIRECTORY ${EXT_BUILD_DIR}
#)
#execute_process(
#  COMMAND ${CMAKE_COMMAND} --build . --target install
#  WORKING_DIRECTORY ${EXT_BUILD_DIR}
#)

#set(CUnit_DIR ${EXT_INSTALL_DIR}/CUnit/share/cmake)


find_package(CUnit REQUIRED)

add_executable(test-program test-program.c)
target_link_libraries(test-program PUBLIC cunit)