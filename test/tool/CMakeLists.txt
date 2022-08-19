
add_executable(success success.c)
add_executable(failure failure.c)

add_test(NAME tool_success_test COMMAND success)

#### comparispawn

add_test(NAME tool_comparispawn_diff_test COMMAND spawn !
  "$<TARGET_FILE:comparispawn>"
  "${CMAKE_SOURCE_DIR}/test/expect/literal_hello.txt"
  "$<TARGET_FILE:shout>" "-" "hel" "lo")
add_test(NAME tool_comparispawn_shout_hello_test COMMAND comparispawn
  "${CMAKE_SOURCE_DIR}/test/expect/literal_hello.txt"
  "$<TARGET_FILE:shout>" "-" "hello")

#### expectish

add_test(NAME tool_expectish_hello_test COMMAND expectish
  "${CMAKE_SOURCE_DIR}/test/expect/literal_hello.txt" "hello")

#### fildespawn

add_test(NAME tool_fildespawn_success_test COMMAND fildespawn -- "$<TARGET_FILE:success>")
add_test(NAME tool_fildespawn_stdin_test COMMAND fildespawn
  "stdin=open_readonly:${CMAKE_SOURCE_DIR}/test/expect/literal_hello.txt"
  -- "$<TARGET_FILE:expectish>" - "hello")

#### pipem_test

add_executable(tool_pipem_test pipem_test.c)
target_link_libraries(tool_pipem_test fildesh_tool_lib)
add_test(NAME tool_pipem_test COMMAND
  tool_pipem_test "$<TARGET_FILE:shout>" "$<TARGET_FILE:expectish>")


#### spawn

add_test(NAME tool_spawn_not_not_test COMMAND
  spawn ! !)
add_test(NAME tool_spawn_spawn_not_not_test COMMAND
  spawn "$<TARGET_FILE:spawn>" ! !)
add_test(NAME tool_spawn_not_spawn_not_test COMMAND
  spawn ! "$<TARGET_FILE:spawn>" !)
add_test(NAME tool_spawn_not_spawn_nothing_test COMMAND
  spawn ! "$<TARGET_FILE:spawn>")
add_test(NAME tool_spawn_not_spawn_absent_test COMMAND
  spawn ! "$<TARGET_FILE:spawn>" "$<TARGET_FILE:spawn>.doesnotexist")
add_test(NAME tool_spawn_not_spawn_not_absent_test COMMAND
  spawn ! "$<TARGET_FILE:spawn>" ! "$<TARGET_FILE:spawn>.doesnotexist")

add_executable(tool_spawn_status_test
  "spawn_status_test.c" "${CMAKE_SOURCE_DIR}/tool/spawn.c")
set_property(TARGET tool_spawn_status_test
  APPEND PROPERTY COMPILE_DEFINITIONS UNIT_TESTING)
add_test(NAME tool_spawn_status_test COMMAND
  tool_spawn_status_test "$<TARGET_FILE:success>" "$<TARGET_FILE:failure>")

