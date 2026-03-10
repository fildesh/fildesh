
add_executable(sxproto_append_test
  "append_test.c")
target_link_libraries(sxproto_append_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_append_test COMMAND sxproto_append_test)

add_executable(sxproto_assign_test
  "assign_test.c")
target_link_libraries(sxproto_assign_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_assign_test COMMAND sxproto_assign_test)

add_executable(sxproto_create_test
  "create_test.c")
target_link_libraries(sxproto_create_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_create_test COMMAND sxproto_create_test)

add_executable(sxproto_parse_test
  "parse_test.c")
target_link_libraries(sxproto_parse_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_parse_test COMMAND sxproto_parse_test)

add_executable(sxproto_parse_with_schema_test
  "parse_with_schema_test.c")
target_link_libraries(sxproto_parse_with_schema_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_parse_with_schema_test COMMAND sxproto_parse_with_schema_test)

add_executable(sxproto_print_content_test
  "print_content_test.c"
  "content/array.c"
  "content/loneof.c"
  "content/manyof.c"
  "content/message.c"
  "content/nest.c"
  "content/string.c"
)
target_link_libraries(sxproto_print_content_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_print_content_test COMMAND
  sxproto_print_content_test
  "${CMAKE_CURRENT_SOURCE_DIR}/content"
)

add_executable(sxproto_toplevel_test
  "toplevel_test.c")
target_link_libraries(sxproto_toplevel_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_toplevel_test COMMAND sxproto_toplevel_test)

add_test(NAME sxpb2json_test COMMAND ${Fildesh_EXECUTABLE}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2json_test.fildesh"
  "--array_json=${CMAKE_CURRENT_SOURCE_DIR}/content/array.json"
  "--array_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/array.sxpb"
  "--loneof_json=${CMAKE_CURRENT_SOURCE_DIR}/content/loneof.json"
  "--loneof_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/loneof.sxpb"
  "--manyof_json=${CMAKE_CURRENT_SOURCE_DIR}/content/manyof.json"
  "--manyof_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/manyof.sxpb"
  "--message_json=${CMAKE_CURRENT_SOURCE_DIR}/content/message.json"
  "--message_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/message.sxpb"
  "--string_json=${CMAKE_CURRENT_SOURCE_DIR}/content/string.json"
  "--string_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/string.sxpb"
)

add_test(NAME sxpb2txtpb_test COMMAND ${Fildesh_EXECUTABLE}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2txtpb_test.fildesh"
  "--array_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/array.sxpb"
  "--array_txtpb=${CMAKE_CURRENT_SOURCE_DIR}/content/array.txtpb"
  "--loneof_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/loneof.sxpb"
  "--loneof_txtpb=${CMAKE_CURRENT_SOURCE_DIR}/content/loneof.txtpb"
  "--manyof_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/manyof.sxpb"
  "--manyof_txtpb=${CMAKE_CURRENT_SOURCE_DIR}/content/manyof.txtpb"
  "--message_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/message.sxpb"
  "--message_txtpb=${CMAKE_CURRENT_SOURCE_DIR}/content/message.txtpb"
  "--string_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/string.sxpb"
  "--string_txtpb=${CMAKE_CURRENT_SOURCE_DIR}/content/string.txtpb"
)

add_test(NAME sxpb2yaml_test COMMAND ${Fildesh_EXECUTABLE}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2yaml_test.fildesh"
  "--array_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/array.sxpb"
  "--array_yaml=${CMAKE_CURRENT_SOURCE_DIR}/content/array.yaml"
  "--loneof_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/loneof.sxpb"
  "--loneof_yaml=${CMAKE_CURRENT_SOURCE_DIR}/content/loneof.yaml"
  "--manyof_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/manyof.sxpb"
  "--manyof_yaml=${CMAKE_CURRENT_SOURCE_DIR}/content/manyof.yaml"
  "--message_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/message.sxpb"
  "--message_yaml=${CMAKE_CURRENT_SOURCE_DIR}/content/message.yaml"
  "--string_sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/string.sxpb"
  "--string_yaml=${CMAKE_CURRENT_SOURCE_DIR}/content/string.yaml"
)
