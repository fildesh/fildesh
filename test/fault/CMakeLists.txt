
add_test(NAME false_test COMMAND spawn ! "$<TARGET_FILE:lace>"
  "${CMAKE_CURRENT_SOURCE_DIR}/false_test.lace")

add_test(NAME include_missing_test COMMAND spawn ! "$<TARGET_FILE:lace>"
  "${CMAKE_CURRENT_SOURCE_DIR}/include_missing_test.lace")

