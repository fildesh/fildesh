
add_test(NAME fault_executable_bytes_of_test COMMAND spawn ! "$<TARGET_FILE:lace>"
  "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_of_test.lace" "$<TARGET_FILE:zec>")

add_test(NAME fault_false_test COMMAND spawn ! "$<TARGET_FILE:lace>"
  "${CMAKE_CURRENT_SOURCE_DIR}/false_test.lace")

add_test(NAME fault_include_missing_test COMMAND spawn ! "$<TARGET_FILE:lace>"
  "${CMAKE_CURRENT_SOURCE_DIR}/include_missing_test.lace")

add_test(NAME fault_pump_executable_bytes_oxf_test COMMAND spawn ! "$<TARGET_FILE:lace>"
  "${CMAKE_CURRENT_SOURCE_DIR}/pump_executable_bytes_oxf_test.lace" "$<TARGET_FILE:zec>")

