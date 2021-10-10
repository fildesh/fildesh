
add_test(NAME fault_executable_bytes_of_test COMMAND spawn ! "$<TARGET_FILE:fildesh>"
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_of_test.fildesh" "$<TARGET_FILE:zec>")

add_test(NAME fault_false_test COMMAND spawn ! "$<TARGET_FILE:fildesh>"
  "${CMAKE_CURRENT_SOURCE_DIR}/false_test.fildesh")

add_test(NAME fault_include_missing_test COMMAND spawn ! "$<TARGET_FILE:fildesh>"
  "${CMAKE_CURRENT_SOURCE_DIR}/include_missing_test.fildesh")

add_test(NAME fault_pump_executable_bytes_oxf_test COMMAND spawn ! "$<TARGET_FILE:fildesh>"
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_executable_bytes_oxf_test.fildesh" "$<TARGET_FILE:zec>")

