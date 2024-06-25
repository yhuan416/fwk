
file(GLOB mmem_debug_adapter_srcs *.c)
add_library(mmem_debug_adapter ${mmem_debug_adapter_srcs})
target_include_directories(mmem_debug_adapter PUBLIC ./)
