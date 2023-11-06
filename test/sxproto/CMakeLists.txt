
add_executable(sxproto_create_test
  "create_test.c")
target_link_libraries(sxproto_create_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_create_test COMMAND sxproto_create_test)

add_executable(sxproto_parse_failure_test
  "parse_failure_test.c")
target_link_libraries(sxproto_parse_failure_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_parse_failure_test COMMAND sxproto_parse_failure_test)

add_executable(sxproto_parse_test
  "parse_test.c")
target_link_libraries(sxproto_parse_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_parse_test COMMAND sxproto_parse_test)

add_executable(sxproto_parse_with_schema_test
  "parse_with_schema_test.c")
target_link_libraries(sxproto_parse_with_schema_test ${FildeshSxproto_LIBRARIES})
add_test(NAME sxproto_parse_with_schema_test COMMAND sxproto_parse_with_schema_test)

add_test(NAME sxpb2json_test COMMAND ${Fildesh_EXECUTABLE}
  -alias "sxpb2json=$<TARGET_FILE:sxpb2json>"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2json_test.fildesh")

add_test(NAME sxpb2txtpb_test COMMAND ${Fildesh_EXECUTABLE}
  -alias "sxpb2txtpb=$<TARGET_FILE:sxpb2txtpb>"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2txtpb_test.fildesh")
