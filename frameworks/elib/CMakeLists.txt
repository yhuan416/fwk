
add_subdirectory(nanopb)

if(BUILD_EXAMPLES)
    add_subdirectory(nanopb/examples/cmake_simple)
endif()

if(BUILD_TEST)
    set(CUNIT_DISABLE_TESTS ON)
    add_subdirectory(cunit-3.2.7)
endif()
