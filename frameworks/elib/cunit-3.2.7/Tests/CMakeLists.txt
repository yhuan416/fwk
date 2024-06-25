add_executable(cicd-fail-setupfuncs
  cicd-fail-setupfuncs.c
  )
target_link_libraries(
  cicd-fail-setupfuncs
  cunit
)

add_test(NAME
  cicd-fail-setupfuncs COMMAND $<TARGET_FILE:cicd-fail-setupfuncs>)
