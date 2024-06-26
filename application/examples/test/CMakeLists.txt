add_executable(test test.c)

target_link_libraries(test PRIVATE
    mmem_debug
    mlist
    easylogger-static
    flashdb
    pthread
)
