add_executable(cembed cembed.c)

add_executable(comparispawn comparispawn.c)
set_property(TARGET comparispawn APPEND PROPERTY INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(comparispawn cx_lib)

add_executable(cswitch cswitch.c)

add_executable(spawn spawn.c)
add_executable(success success.c)
add_executable(failure failure.c)
add_test(NAME success_test COMMAND spawn "$<TARGET_FILE:success>")
add_test(NAME failure_test COMMAND spawn ! "$<TARGET_FILE:failure>")
