
#### comparispawn

add_test(NAME comparispawn_diff_test COMMAND spawn !
  "$<TARGET_FILE:comparispawn>"
  "${CMAKE_SOURCE_DIR}/test/expect/hello.txt"
  "$<TARGET_FILE:shout>" "-" "hel" "lo")
add_test(NAME comparispawn_shout_hello_test COMMAND comparispawn
  "${CMAKE_SOURCE_DIR}/test/expect/hello.txt"
  "$<TARGET_FILE:shout>" "-" "hello")

#### expectish

add_test(NAME expectish_hello_test COMMAND expectish "${CMAKE_CURRENT_SOURCE_DIR}/../expect/hello.txt" "hello")

#### pipem_test

add_executable(pipem_test pipem_test.c)
target_link_libraries(pipem_test lace_tool_lib)
add_test(NAME pipem_test COMMAND pipem_test "$<TARGET_FILE:shout>" "$<TARGET_FILE:expectish>")


#### spawn

add_executable(success success.c)
add_executable(failure failure.c)

add_test(NAME success_test COMMAND success)

add_test(NAME spawn_not_not_test COMMAND spawn ! !)
add_test(NAME spawn_spawn_not_not_test COMMAND spawn "$<TARGET_FILE:spawn>" ! !)
add_test(NAME spawn_not_spawn_not_test COMMAND spawn ! "$<TARGET_FILE:spawn>" !)
add_test(NAME spawn_not_spawn_nothing_test COMMAND spawn ! "$<TARGET_FILE:spawn>")
add_test(NAME spawn_not_spawn_absent_test COMMAND spawn ! "$<TARGET_FILE:spawn>" "$<TARGET_FILE:spawn>.doesnotexist")
add_test(NAME spawn_not_spawn_not_absent_test COMMAND spawn ! "$<TARGET_FILE:spawn>" ! "$<TARGET_FILE:spawn>.doesnotexist")

add_executable(spawn_status_test "spawn_status_test.c" "${CMAKE_SOURCE_DIR}/tool/spawn.c")
set_property(TARGET spawn_status_test
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_TOOL_LIBRARY)
add_test(NAME spawn_status_test COMMAND spawn_status_test "$<TARGET_FILE:success>" "$<TARGET_FILE:failure>")

