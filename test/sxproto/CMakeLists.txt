
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

add_test(NAME sxpb2json_test COMMAND ${Fildesh_EXECUTABLE}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2json_test.fildesh"
  "--sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/message.sxpb"
  "--json=${CMAKE_CURRENT_SOURCE_DIR}/content/message.json"
)

add_test(NAME sxpb2txtpb_test COMMAND ${Fildesh_EXECUTABLE}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2txtpb_test.fildesh"
  "--sxpb=${CMAKE_CURRENT_SOURCE_DIR}/content/message.sxpb"
  "--txtpb=${CMAKE_CURRENT_SOURCE_DIR}/content/message.txtpb"
)
