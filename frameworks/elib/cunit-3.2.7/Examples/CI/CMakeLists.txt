project(CI-Examples C)

add_executable(cicd-pass-setupfuncs
        cicd-pass-setupfuncs.c
        )
target_link_libraries(
        cicd-pass-setupfuncs
        cunit
)

add_executable(cicd-pass-plain
        cicd-pass-plain.c
        )
target_link_libraries(
        cicd-pass-plain
        cunit
)

add_executable(cicd-multi-suite
        cicd-multi-suite.c
        )
target_link_libraries(
        cicd-multi-suite
        cunit
)

add_executable(cicd-skips
        cicd-skips.c
        )
target_link_libraries(
        cicd-skips
        cunit
)

add_executable(cicd-skip-suite
        cicd-skip-suite.c
        )
target_link_libraries(
        cicd-skip-suite
        cunit
)

add_test(NAME
  cicd-pass-plain COMMAND $<TARGET_FILE:cicd-pass-plain>)
add_test(NAME
  cicd-multi-suite COMMAND $<TARGET_FILE:cicd-multi-suite>)
add_test(NAME
  cicd-pass-setupfuncs COMMAND $<TARGET_FILE:cicd-pass-setupfuncs>)
add_test(NAME
  cicd-skips COMMAND $<TARGET_FILE:cicd-skips>)
add_test(NAME
  cicd-skip-suite COMMAND $<TARGET_FILE:cicd-skip-suite>)

