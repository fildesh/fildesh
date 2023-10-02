
add_test(NAME sxpb2txtpb_test COMMAND fildesh
  -alias "sxpb2txtpb=$<TARGET_FILE:sxpb2txtpb>"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxpb2txtpb_test.fildesh")
