
if (CMAKE_C_COMPILER_ID STREQUAL TCC_COMPILER_ID)
  # TinyCC only compiles C. Assume no C++ compiler.
  return()
endif()

add_executable(cc_ifstream_test "ifstream_test.cc")
target_link_libraries(cc_ifstream_test fildesh_lib)
add_test(NAME cc_ifstream_test COMMAND cc_ifstream_test
  "${CMAKE_SOURCE_DIR}/test/expect/literal_hello.txt"
  "${CMAKE_SOURCE_DIR}/test/expect/literal_127.txt"
  )
