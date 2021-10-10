
add_test(NAME syntax_concatenate_oooxfxax_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_oooxfxax_test.fildesh")
add_test(NAME syntax_concatenate_forkonly_oooxfxax_test COMMAND
  fildesh -forkonly -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_oooxfxax_test.fildesh")

add_test(NAME syntax_executable_bytes_oxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_oxf_test.fildesh" "$<TARGET_FILE:zec>")
add_test(NAME syntax_executable_bytes_ofxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_ofxf_test.fildesh" "$<TARGET_FILE:zec>")

add_test(NAME syntax_pump_ofxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ofxf_test.fildesh")
add_test(NAME syntax_pump_oxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_oxf_test.fildesh")
add_test(NAME syntax_pump_ofx_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ofx_test.fildesh")
add_test(NAME syntax_pump_ox_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ox_test.fildesh")

