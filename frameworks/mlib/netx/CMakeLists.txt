
project(netx C)

add_library(netx STATIC
    netx.c
    netx.h
)
target_include_directories(netx PUBLIC .)

target_compile_definitions(netx PUBLIC _epoll _getifaddrs)
