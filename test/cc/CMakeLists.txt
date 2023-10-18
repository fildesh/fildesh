
if (CMAKE_C_COMPILER_ID STREQUAL TCC_COMPILER_ID)
  # TinyCC only compiles C. Assume no C++ compiler.
  return()
endif()

add_executable(cc_istream_test "istream_test.cc")
target_link_libraries(cc_istream_test PRIVATE ${Fildesh_LIBRARIES})
add_test(NAME cc_istream_test COMMAND cc_istream_test)

add_executable(cc_ostream_test "ostream_test.cc")
target_link_libraries(cc_ostream_test PRIVATE ${Fildesh_LIBRARIES})
add_test(NAME cc_ostream_test COMMAND cc_ostream_test)

add_executable(cc_string_test "string_test.cc")
target_link_libraries(cc_string_test PRIVATE ${Fildesh_LIBRARIES})
add_test(NAME cc_string_test COMMAND cc_string_test)
