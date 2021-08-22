
add_test(NAME syntax_concatenate_oooxfxax_test COMMAND lace -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_oooxfxax_test.lace")
add_test(NAME syntax_concatenate_forkonly_oooxfxax_test COMMAND
  lace -forkonly -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_oooxfxax_test.lace")

add_test(NAME syntax_executable_bytes_oxf_test COMMAND lace -f
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_oxf_test.lace" "$<TARGET_FILE:zec>")
add_test(NAME syntax_executable_bytes_ofxf_test COMMAND lace -f
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_ofxf_test.lace" "$<TARGET_FILE:zec>")

add_test(NAME syntax_pump_ofxf_test COMMAND lace -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ofxf_test.lace")
add_test(NAME syntax_pump_oxf_test COMMAND lace -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_oxf_test.lace")
add_test(NAME syntax_pump_ofx_test COMMAND lace -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ofx_test.lace")
add_test(NAME syntax_pump_ox_test COMMAND lace -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ox_test.lace")

