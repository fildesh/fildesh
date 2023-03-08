
add_test(NAME sxproto2textproto_test COMMAND fildesh
  -alias "sxproto2textproto=$<TARGET_FILE:sxproto2textproto>"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/sxproto2textproto_test.fildesh")
