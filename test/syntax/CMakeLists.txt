
if (UNIX)
  add_test(NAME syntax_alias_ofxf_test COMMAND fildesh
    -alias "expectish=$<TARGET_FILE:expectish>"
    -alias "shout=$<TARGET_FILE:shout>"
    -f "${CMAKE_CURRENT_SOURCE_DIR}/alias_ofxf_test.fildesh")
endif()

add_test(NAME syntax_coalesce_undefined_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/coalesce_undefined_test.fildesh")

add_test(NAME syntax_concatenate_axfxax_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_axfxax_test.fildesh")
add_test(NAME syntax_concatenate_hxfxax_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_hxfxax_test.fildesh")

add_test(NAME syntax_concatenate_oooxfxax_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_oooxfxax_test.fildesh")
add_test(NAME syntax_concatenate_forkonly_oooxfxax_test COMMAND
  fildesh -forkonly -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_oooxfxax_test.fildesh")

add_test(NAME syntax_concatenate_inline_hhh_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_inline_hhh_test.fildesh")
add_test(NAME syntax_concatenate_inline_named_arg_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/concatenate_inline_named_arg_test.fildesh")

add_test(NAME syntax_dev_null_o_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/dev_null_o_test.fildesh")
add_test(NAME syntax_dev_stderr_ox_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/dev_stderr_ox_test.fildesh")

add_test(NAME syntax_environment_variable_xaxa_test COMMAND
  "${CMAKE_COMMAND}" -E env "VARIABLE_A=hello" "VARIABLE_B=world"
  "$<TARGET_FILE:fildesh>" -f
  "${CMAKE_CURRENT_SOURCE_DIR}/environment_variable_xaxa_test.fildesh")

add_test(NAME syntax_escape_string_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/escape_string_test.fildesh")

add_test(NAME syntax_executable_bytes_oxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_oxf_test.fildesh"
  "--input_exe=$<TARGET_FILE:zec>"
)
add_test(NAME syntax_executable_bytes_ofxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_ofxf_test.fildesh"
  "--input_exe=$<TARGET_FILE:zec>"
)

add_test(NAME syntax_expect_failure_oxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/expect_failure_oxf_test.fildesh")
add_test(NAME syntax_expect_failure_message_oxoxoxox_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/expect_failure_message_oxoxoxox_test.fildesh")

add_test(NAME syntax_filename_xofxof_test COMMAND
  "${CMAKE_COMMAND}" -E env "TEST_TMPDIR=${CMAKE_CURRENT_BINARY_DIR}"
  "$<TARGET_FILE:fildesh>" -f
  "${CMAKE_CURRENT_SOURCE_DIR}/filename_xofxof_test.fildesh")

add_test(NAME syntax_multiline_string_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/multiline_string_test.fildesh")

add_test(NAME syntax_pump_ofxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ofxf_test.fildesh")
add_test(NAME syntax_pump_oxf_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_oxf_test.fildesh")
add_test(NAME syntax_pump_ofx_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ofx_test.fildesh")
add_test(NAME syntax_pump_ox_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_ox_test.fildesh")

add_test(NAME syntax_redefine_hhxhhx_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/redefine_hhxhhx_test.fildesh")
add_test(NAME syntax_redefine_hxahxfhxh_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/redefine_hxahxfhxh_test.fildesh")

add_test(NAME syntax_stderr_ooox_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/stderr_ooox_test.fildesh")

add_test(NAME syntax_tmpfile_xofxof_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/tmpfile_xofxof_test.fildesh")

add_test(NAME syntax_wait_xaxaxa_test COMMAND
  "${CMAKE_COMMAND}" -E env "TEST_TMPDIR=${CMAKE_CURRENT_BINARY_DIR}"
  "$<TARGET_FILE:fildesh>" -f
  "${CMAKE_CURRENT_SOURCE_DIR}/wait_xaxaxa_test.fildesh")
add_test(NAME syntax_forkonly_wait_xaxaxa_test COMMAND
  "${CMAKE_COMMAND}" -E env "TEST_TMPDIR=${CMAKE_CURRENT_BINARY_DIR}"
  "$<TARGET_FILE:fildesh>" -forkonly -f
  "${CMAKE_CURRENT_SOURCE_DIR}/wait_xaxaxa_test.fildesh")
