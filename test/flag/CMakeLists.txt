add_test(NAME flag_named_arg_test COMMAND fildesh 
  -a "hello=Hello" -a "world=World"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/named_arg_test.fildesh")

add_test(NAME flag_setenv_test COMMAND fildesh 
  -setenv "HELLO=Hello" -setenv "WORLD=World"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/setenv_test.fildesh")
