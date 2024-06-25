add_executable(server server.c)
target_link_libraries(server netx)

add_executable(client client.c)
target_link_libraries(client netx)