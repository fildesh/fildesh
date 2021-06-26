
#add_test(NAME elastic_catfile_test COMMAND
#  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
#  ${lace_exe} --
#  "|< zec"
#  "|- waitdo -- rm -f ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- elastic -x ${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt -o ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- zec ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|> stdout"
#  )

#### add

add_test(NAME lace_as_add_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/summed_integers.txt"
  "${lace_exe}" -stdinfile "${CMAKE_CURRENT_SOURCE_DIR}/integers.txt"
  -as add
  )

#### cmp

add_test(NAME cmp_equal_test COMMAND cmp
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt")

add_test(NAME cmp_diff_test COMMAND spawn ! "$<TARGET_FILE:cmp>"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline_x2.txt")

#### elastic

add_test(NAME elastic_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/elastic_cat.lace"
  )

add_test(NAME elastic_tee_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline_x2.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/elastic_tee.lace"
  )

#### time2sec

add_executable(time2sec_test "time2sec_test.c" "${CMAKE_SOURCE_DIR}/builtin/time2sec.c")
target_link_libraries(time2sec_test lace_lib lace_tool_lib)
set_property(TARGET time2sec_test
  APPEND PROPERTY COMPILE_DEFINITIONS MAIN_LACE_EXECUTABLE)
add_test(NAME time2sec_test COMMAND time2sec_test)

#### void

add_executable(void_test "void_test.c" "${CMAKE_SOURCE_DIR}/builtin/void.c")
target_link_libraries(void_test lace_lib lace_tool_lib)
set_property(TARGET void_test
  APPEND PROPERTY COMPILE_DEFINITIONS MAIN_LACE_EXECUTABLE)
add_test(NAME void_test COMMAND void_test)

#### xpipe

if (UNIX)
  add_test(NAME xpipe_nested_test COMMAND
    comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/../expect/xpipe_nested.txt"
    ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/xpipe_nested.lace"
    )
endif()

#### zec

add_test(NAME zec_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/zec_cat.lace"
  )

# Only CMake test here that actually uses `spawn`.
# However, Bazel tests rely on it heavily.
add_test(NAME zec_nop_test COMMAND
  spawn ${zec_exe} "/" "/"
  )

add_test(NAME zec_usage_test COMMAND
  spawn ${zec_exe} -h
  )
