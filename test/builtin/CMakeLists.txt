
add_test (NAME elastic_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/elastic_cat.lace"
  )

add_test (NAME xpipe_nested_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/../expect/xpipe_nested.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/xpipe_nested.lace"
  )

add_test (NAME zec_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/zec_cat.lace"
  )
