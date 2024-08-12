
add_subdirectory(kcp)
include_directories(kcp)

add_executable(recv recv.c)

target_link_libraries(recv PRIVATE
    osal
    pthread
    kcp
)

add_executable(send send.c)

target_link_libraries(send PRIVATE
    osal
    pthread
    kcp
)
