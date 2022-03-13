add_test(NAME flag_named_arg_test COMMAND fildesh 
  -a "hello=Hello" -a "world=World"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/named_arg_test.fildesh")
