cmake_minimum_required (VERSION 3.2)

project(flashdb C)

set(flashdb_VERSION 0.0.1)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

set(ROOTPATH ${CMAKE_CURRENT_SOURCE_DIR}/FlashDB)

file(GLOB FLASHDB_SOURCES ${ROOTPATH}/src/*.c)
file(GLOB FLASHDB_SAMPLES_SOURCES ${ROOTPATH}/samples/*.c)

set(FLASHDB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/port ${ROOTPATH}/inc)

add_library(flashdb ${FLASHDB_SOURCES} ${FLASHDB_SAMPLES_SOURCES})
target_include_directories(flashdb PUBLIC ${FLASHDB_INCLUDE_DIRS})

# target_link_libraries(flashdb PRIVATE pthread)
target_compile_options(flashdb PRIVATE -fPIC)
