
if (FILDESH_BUILTIN_ELASTIC_AIO_ON)
  add_executable(chatty "chatty.c")
  target_link_libraries(chatty fildesh_lib ${FILDESH_AIO_LIBS})
  set_property(TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=201112L)

  add_test(NAME src_chatty_test COMMAND chatty)
endif()

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
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

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

add_executable(urandom_variety_test
  "urandom_variety_test.c")
target_link_libraries(urandom_variety_test fildesh_lib)
add_test(NAME urandom_variety_test COMMAND urandom_variety_test)
