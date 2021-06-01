
#add_test(NAME elastic_catfile_test COMMAND
#  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
#  ${lace_exe} --
#  "|< zec"
#  "|- waitdo -- rm -f ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- elastic -x ${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt -o ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- zec ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|> stdout"
#  )

add_test(NAME elastic_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/elastic_cat.lace"
  )

add_test(NAME elastic_tee_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline_x2.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/elastic_tee.lace"
  )

add_test(NAME lace_as_add_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/summed_integers.txt"
  "${lace_exe}" -stdinfile "${CMAKE_CURRENT_SOURCE_DIR}/integers.txt"
  -as add
  )

add_test(NAME xpipe_nested_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/../expect/xpipe_nested.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/xpipe_nested.lace"
  )

add_test(NAME zec_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/zec_cat.lace"
  )

# Only CMake test here that actually uses `spawn`.
# However, Bazel tests rely on it heavily.
add_test(NAME zec_nop_test COMMAND
  spawn ${zec_exe} "/" "/"
  )
