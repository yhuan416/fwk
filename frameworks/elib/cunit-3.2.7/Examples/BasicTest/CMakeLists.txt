project(Examples-BasicTest C)

# This test is supposed to fail with:
# ---
# Run Summary:    Type  Total    Ran Passed Failed Inactive
#               suites     16     14    n/a      3        0
#                tests     35     31     10     21        0
#              asserts     89     89     47     42      n/a

add_executable(t-ex-basictest
        BasicTest.c
        ${CMAKE_CURRENT_LIST_DIR}/../ExampleTests.c
        )

target_link_libraries(t-ex-basictest
        cunit
        )

target_include_directories(t-ex-basictest
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/..)