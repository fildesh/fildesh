
if (UNIX AND NOT APPLE)
  add_test(NAME chatty_test COMMAND chatty)
endif()

add_executable(alloc_test
  "alloc_test.c")
target_link_libraries(alloc_test fildesh_lib)
add_test(NAME alloc_test COMMAND alloc_test)

add_executable(delimit_edge_test
  "delimit_edge_test.c")
target_link_libraries(delimit_edge_test fildesh_lib)
add_test(NAME delimit_edge_test COMMAND delimit_edge_test)

add_executable(getline_test
  "getline_test.c")
target_link_libraries(getline_test fildesh_lib)
add_test(NAME getline_test COMMAND getline_test)

add_executable(gets_test
  "gets_test.c")
target_link_libraries(gets_test fildesh_lib)
add_test(NAME gets_test COMMAND gets_test)

add_executable(kve_test
  "kve_test.c")
target_link_libraries(kve_test fildesh_lib)
target_include_directories(kve_test PRIVATE "${CMAKE_SOURCE_DIR}")
add_test(NAME kve_test COMMAND kve_test)

add_executable(mascii_test
  "mascii_test.c")
target_link_libraries(mascii_test fildesh_lib)
target_include_directories(mascii_test PRIVATE "${CMAKE_SOURCE_DIR}")
add_test(NAME mascii_test COMMAND mascii_test)

add_executable(parse_int_test
  "parse_int_test.c")
target_link_libraries(parse_int_test fildesh_lib)
add_test(NAME parse_int_test COMMAND parse_int_test)

add_executable(puts_test
  "puts_test.c")
target_link_libraries(puts_test fildesh_lib)
add_test(NAME puts_test COMMAND puts_test)

add_executable(read_file_lines_test
  "read_file_lines_test.c")
target_link_libraries(read_file_lines_test fildesh_lib)
add_test(NAME read_file_lines_test
  COMMAND read_file_lines_test
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_executable(sibling_file_test
  "sibling_file_test.c")
target_link_libraries(sibling_file_test fildesh_lib)
add_test(NAME sibling_file_test COMMAND
  "${CMAKE_COMMAND}" -E env "TEST_TMPDIR=${CMAKE_CURRENT_BINARY_DIR}" "$<TARGET_FILE:sibling_file_test>")

add_executable(size_test
  "size_test.c")
target_link_libraries(size_test fildesh_lib)
add_test(NAME size_test COMMAND size_test)

add_executable(skipstr_test
  "skipstr_test.c")
target_link_libraries(skipstr_test fildesh_lib)
add_test(NAME skipstr_test COMMAND skipstr_test)

add_executable(slicechrs_test
  "slicechrs_test.c")
target_link_libraries(slicechrs_test fildesh_lib)
add_test(NAME slicechrs_test COMMAND slicechrs_test)
