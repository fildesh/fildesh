add_test(NAME flag_named_arg_test COMMAND fildesh
  -a "hello=Hello" -a "world=World"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/named_arg_test.fildesh")

add_test(NAME flag_nullary_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/nullary_test.fildesh")

add_test(NAME flag_filename_option_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/option_filename_test.fildesh")

add_test(NAME flag_option_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/option_test.fildesh")

add_test(NAME flag_setenv_test COMMAND fildesh
  -setenv "HELLO=Hello" -setenv "WORLD=World"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/setenv_test.fildesh")

add_test(NAME flag_stdargz_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/stdargz_test.fildesh")

add_test(NAME flag_stdio_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/stdio_test.fildesh")

add_test(NAME flag_version_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/version_test.fildesh"
  "--bazel_module_file=${PROJECT_SOURCE_DIR}/MODULE.bazel")
