#
# clone this repo to esp project root directory
#
# Add this line to Project CMakeLists.txt:
#
#   include(${CMAKE_CURRENT_SOURCE_DIR}/fwk/frameworks/cmake/esp-idf.cmake)
#
#

set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/fwk/frameworks/mlib/osal
    ${CMAKE_CURRENT_SOURCE_DIR}/fwk/frameworks/mlib/mlist
)
