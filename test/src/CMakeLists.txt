
add_executable(getline_test
  "getline_test.c")
target_link_libraries(getline_test lace_lib)
add_test(NAME getline_test COMMAND getline_test)

add_executable(read_file_lines_test
  "read_file_lines_test.c")
target_link_libraries(read_file_lines_test lace_lib)
add_test(NAME read_file_lines_test
  COMMAND read_file_lines_test
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_executable(size_test
  "size_test.c")
target_link_libraries(size_test lace_lib)
add_test(NAME size_test COMMAND size_test)
