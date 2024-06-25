add_library(cunitexamples ExampleTests.c)
target_include_directories(cunitexamples PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(cunitexamples cunit)

add_subdirectory(BasicTest)
add_subdirectory(AutomatedTest)
add_subdirectory(CI)