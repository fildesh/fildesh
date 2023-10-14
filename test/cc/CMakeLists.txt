
if (CMAKE_C_COMPILER_ID STREQUAL TCC_COMPILER_ID)
  # TinyCC only compiles C. Assume no C++ compiler.
  return()
endif()

add_executable(cc_ifstream_test "ifstream_test.cc")
target_link_libraries(cc_ifstream_test fildesh_lib)
add_test(NAME cc_ifstream_test COMMAND cc_ifstream_test
  "${PROJECT_SOURCE_DIR}/test/expect/literal_hello.txt"
  "${PROJECT_SOURCE_DIR}/test/expect/literal_127.txt"
  )

add_executable(cc_string_test "string_test.cc")
target_link_libraries(cc_string_test fildesh_lib)
add_test(NAME cc_string_test COMMAND cc_string_test)
