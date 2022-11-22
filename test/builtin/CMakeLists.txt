
#add_test(NAME elastic_catfile_test COMMAND
#  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
#  ${fildesh_exe} --
#  "|< zec"
#  "|- waitdo -- rm -f ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- elastic -x ${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt -o ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- zec ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|> stdout"
#  )

#### add

add_executable(builtin_add_float_test "add_float_test.c" "${CMAKE_SOURCE_DIR}/src/builtin/add.c")
target_link_libraries(builtin_add_float_test fildesh_lib fildesh_tool_lib)
set_property(TARGET builtin_add_float_test
  APPEND PROPERTY COMPILE_DEFINITIONS FILDESH_BUILTIN_LIBRARY)
add_test(NAME builtin_add_float_test COMMAND builtin_add_float_test)

#### bestmatch

add_executable(builtin_bestmatch_test "bestmatch_test.c")
target_link_libraries(builtin_bestmatch_test fildesh_lib fildesh_tool_lib)
add_test(NAME builtin_bestmatch_test COMMAND
  builtin_bestmatch_test "$<TARGET_FILE:bestmatch>")

add_test(NAME builtin_bestmatch_edge_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/bestmatch_edge_test.fildesh")

#### cmp

add_test(NAME builtin_cmp_equal_test COMMAND cmp
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  "-o" "-")

add_test(NAME builtin_cmp_diff_test COMMAND spawn ! "$<TARGET_FILE:cmp>"
  "-o" "-"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline_x2.txt")

#### elastic

add_test(NAME builtin_elastic_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${fildesh_exe} -f "${CMAKE_CURRENT_SOURCE_DIR}/elastic_cat.fildesh"
  )

add_test(NAME builtin_elastic_tee_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline_x2.txt"
  ${fildesh_exe} -f "${CMAKE_CURRENT_SOURCE_DIR}/elastic_tee.fildesh"
  )

add_executable(builtin_elastic_alt_tee_test "elastic_alt_tee_test.c")
target_link_libraries(builtin_elastic_alt_tee_test fildesh_lib fildesh_tool_lib)
add_test(NAME builtin_elastic_pthread_tee_test COMMAND
  builtin_elastic_alt_tee_test "$<TARGET_FILE:elastic_pthread>")
if (UNIX)
  add_test(NAME builtin_elastic_poll_tee_test COMMAND
    builtin_elastic_alt_tee_test "$<TARGET_FILE:elastic_poll>")
  if (NOT APPLE AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_test(NAME builtin_elastic_aio_tee_test COMMAND
      builtin_elastic_alt_tee_test "$<TARGET_FILE:elastic_aio>")
  endif()
endif()

#### execfd

add_executable(builtin_execfd_test "execfd_test.c" "${CMAKE_SOURCE_DIR}/src/execfd.c")
target_link_libraries(builtin_execfd_test fildesh_lib fildesh_tool_lib)
set_property(TARGET builtin_execfd_test
  APPEND PROPERTY COMPILE_DEFINITIONS UNIT_TESTING)
add_test(NAME builtin_execfd_test COMMAND
  "${CMAKE_COMMAND}" -E env "TEST_TMPDIR=${CMAKE_CURRENT_BINARY_DIR}"
  "$<TARGET_FILE:builtin_execfd_test>" "$<TARGET_FILE:zec>")

add_executable(builtin_execfd_cat_test "execfd_cat_test.c" "${CMAKE_SOURCE_DIR}/src/execfd.c")
target_link_libraries(builtin_execfd_cat_test fildesh_lib fildesh_tool_lib)
set_property(TARGET builtin_execfd_cat_test
  APPEND PROPERTY COMPILE_DEFINITIONS UNIT_TESTING)
add_test(NAME builtin_execfd_cat_test COMMAND
  builtin_execfd_cat_test "$<TARGET_FILE:shout>")

add_test(NAME builtin_execfd_script_test COMMAND
  fildesh -f "${CMAKE_CURRENT_SOURCE_DIR}/execfd_script_test.fildesh" "$<TARGET_FILE:zec>")

#### expect_failure

add_executable(builtin_expect_failure_test "expect_failure_test.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/expect_failure.c")
target_link_libraries(builtin_expect_failure_test fildesh_lib)
set_property(TARGET builtin_expect_failure_test
  APPEND PROPERTY COMPILE_DEFINITIONS UNIT_TESTING)
add_test(NAME builtin_expect_failure_test COMMAND
  builtin_expect_failure_test
  "${CMAKE_SOURCE_DIR}/test/expect/literal_0.txt"
  "${CMAKE_SOURCE_DIR}/test/expect/literal_127.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  )

#### fildesh

add_test(NAME builtin_fildesh_as_add_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/summed_integers.txt"
  "${fildesh_exe}" -stdin "${CMAKE_CURRENT_SOURCE_DIR}/integers.txt"
  -as add
  )

add_executable(builtin_fildesh_cli_hello_test "fildesh_cli_hello_test.c")
target_link_libraries(builtin_fildesh_cli_hello_test fildesh_lib fildesh_tool_lib)
add_test(NAME builtin_fildesh_cli_hello_test COMMAND builtin_fildesh_cli_hello_test "${fildesh_exe}")

add_executable(builtin_fildesh_usage_test "fildesh_usage_test.c")
target_link_libraries(builtin_fildesh_usage_test fildesh_compat_lib)
add_test(NAME builtin_fildesh_usage_test COMMAND builtin_fildesh_usage_test "${fildesh_exe}")

#### godo

add_executable(builtin_godo_test "godo_test.c" "${CMAKE_SOURCE_DIR}/src/godo.c")
target_link_libraries(builtin_godo_test fildesh_lib)
set_property(TARGET builtin_godo_test
  APPEND PROPERTY COMPILE_DEFINITIONS FILDESH_BUILTIN_LIBRARY)
add_test(NAME builtin_godo_test COMMAND
  builtin_godo_test "$<TARGET_FILE:expectish>" "${CMAKE_SOURCE_DIR}/test/expect/literal_hello.txt")

#### replace_string

add_test(NAME builtin_replace_string_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/replace_string_test.fildesh")

#### sponge

if (UNIX)
  add_test(NAME builtin_sponge_sh_test COMMAND
    "${CMAKE_CURRENT_SOURCE_DIR}/sponge_test.sh" "$<TARGET_FILE:sponge>")
endif()

#### sshall

add_executable(echocat "echocat.c" "${CMAKE_SOURCE_DIR}/src/builtin/zec.c")
set_property(TARGET echocat
  APPEND PROPERTY COMPILE_DEFINITIONS FILDESH_BUILTIN_LIBRARY)
target_link_libraries(echocat fildesh_lib)

add_test(NAME builtin_echocat_test COMMAND
  fildesh -alias "echocat=$<TARGET_FILE:echocat>" -f
  "${CMAKE_CURRENT_SOURCE_DIR}/echocat_test.fildesh")

add_executable(builtin_sshall_test "sshall_test.c")
target_link_libraries(builtin_sshall_test fildesh_lib fildesh_tool_lib)
add_test(NAME builtin_sshall_test COMMAND
  builtin_sshall_test "$<TARGET_FILE:sshall>" "$<TARGET_FILE:echocat>")

#### time2sec

add_executable(builtin_time2sec_test "time2sec_test.c" "${CMAKE_SOURCE_DIR}/src/builtin/time2sec.c")
target_link_libraries(builtin_time2sec_test fildesh_lib fildesh_tool_lib)
set_property(TARGET builtin_time2sec_test
  APPEND PROPERTY COMPILE_DEFINITIONS FILDESH_BUILTIN_LIBRARY)
add_test(NAME builtin_time2sec_test COMMAND builtin_time2sec_test)

#### transpose

add_executable(builtin_transpose_test "transpose_test.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/transpose.c")
target_link_libraries(builtin_transpose_test fildesh_lib fildesh_tool_lib)
set_property(TARGET builtin_transpose_test
  APPEND PROPERTY COMPILE_DEFINITIONS UNIT_TESTING)
add_test(NAME builtin_transpose_test COMMAND builtin_transpose_test)

add_test(NAME builtin_transpose_tab_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/transpose_tab_test.fildesh")

#### ujoin

add_executable(builtin_ujoin_test "ujoin_test.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/ujoin.c")
target_link_libraries(builtin_ujoin_test fildesh_lib fildesh_tool_lib)
set_property(TARGET builtin_ujoin_test
  APPEND PROPERTY COMPILE_DEFINITIONS UNIT_TESTING)
add_test(NAME builtin_ujoin_test COMMAND builtin_ujoin_test)

add_test(NAME builtin_ujoin_misdup_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/ujoin_misdup_test.fildesh")

#### void

add_executable(builtin_void_test "void_test.c")
target_link_libraries(builtin_void_test fildesh_lib fildesh_tool_lib)
add_test(NAME builtin_void_test COMMAND builtin_void_test "$<TARGET_FILE:void>")

#### waitdo

add_executable(builtin_waitdo_test "waitdo_test.c")
target_link_libraries(builtin_waitdo_test fildesh_lib fildesh_tool_lib)
add_test(NAME builtin_waitdo_test COMMAND
  builtin_waitdo_test "$<TARGET_FILE:waitdo>" "$<TARGET_FILE:shout>")

#### xpipe

add_test(NAME builtin_xpipe_nested_test COMMAND fildesh -f
  "${CMAKE_CURRENT_SOURCE_DIR}/xpipe_nested_test.fildesh")

#### zec

add_test(NAME builtin_zec_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${fildesh_exe} -f "${CMAKE_CURRENT_SOURCE_DIR}/zec_cat.fildesh"
  )

# Only CMake test here that actually uses `spawn`.
# However, Bazel tests rely on it heavily.
add_test(NAME builtin_zec_nop_test COMMAND
  spawn ${zec_exe} "/" "/"
  )
