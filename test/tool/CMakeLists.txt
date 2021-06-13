
add_executable(success success.c)
add_executable(failure failure.c)

add_test(NAME success_test COMMAND success)
add_test(NAME spawn_success_test COMMAND spawn "$<TARGET_FILE:success>")
add_test(NAME spawn_not_failure_test COMMAND spawn ! "$<TARGET_FILE:failure>")
add_test(NAME spawn_not_spawn_failure_test COMMAND spawn ! "$<TARGET_FILE:spawn>" "$<TARGET_FILE:failure>")
add_test(NAME spawn_not_spawn_not_success_test COMMAND spawn ! "$<TARGET_FILE:spawn>" ! "$<TARGET_FILE:success>")
add_test(NAME spawn_not_spawn_invalid_test COMMAND spawn ! "$<TARGET_FILE:spawn>" .)
add_test(NAME spawn_not_spawn_not_invalid_test COMMAND spawn ! "$<TARGET_FILE:spawn>" ! .)
add_test(NAME spawn_not_spawn_absent_test COMMAND spawn ! "$<TARGET_FILE:spawn>" "$<TARGET_FILE:spawn>.doesnotexist")
add_test(NAME spawn_not_spawn_not_absent_test COMMAND spawn ! "$<TARGET_FILE:spawn>" ! "$<TARGET_FILE:spawn>.doesnotexist")
