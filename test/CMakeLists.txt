cmake_minimum_required(VERSION 3.1)

include_directories("../src")

file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
          ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

        message(STATUS ${TEST_SRCS})
add_executable(quant_test ${TEST_SRCS})
target_link_libraries(quant_test quant ${LIBUV_LIBRARY})

add_test(NAME quant_unit_test COMMAND quant_test -s)
