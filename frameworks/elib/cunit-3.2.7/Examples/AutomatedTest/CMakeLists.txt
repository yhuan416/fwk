add_executable(t-automated
        AutomatedTest.c
        )
target_link_libraries(t-automated cunitexamples)

add_test(NAME
  t-automated COMMAND $<TARGET_FILE:t-automated>)