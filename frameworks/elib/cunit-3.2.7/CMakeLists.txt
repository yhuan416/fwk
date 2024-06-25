# Static file is generated/linked against for this project
#   Fixes issues with toolchains which only support static linking
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(CUnit C)

cmake_minimum_required (VERSION 3.2)

# If the user hasn't specified a preference for disabling
# tests/examples, default to leaving them enabled.
if (NOT DEFINED CUNIT_DISABLE_TESTS)
  set (CUNIT_DISABLE_TESTS FALSE)
endif ()
if (NOT DEFINED CUNIT_DISABLE_EXAMPLES)
  set (CUNIT_DISABLE_EXAMPLES FALSE)
endif ()

set(CUNIT_DISABLE_TESTS "${CUNIT_DISABLE_TESTS}" CACHE INTERNAL "Disable CUnit tests")
set(CUNIT_DISABLE_EXAMPLES "${CUNIT_DISABLE_EXAMPLES}" CACHE INTERNAL "Disable CUnit examples")

enable_testing()
set(CUNIT_CMAKE_INSTALL yes)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Werror=strict-prototypes")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /WX /wd4996")
endif()

add_subdirectory(CUnit)
if(NOT CUNIT_DISABLE_TESTS)
  add_subdirectory(Tests)
endif()

if(NOT CUNIT_DISABLE_EXAMPLES)
  add_subdirectory(Examples)
endif()
