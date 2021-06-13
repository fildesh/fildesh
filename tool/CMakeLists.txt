add_executable(cembed cembed.c)

add_executable(comparispawn comparispawn.c)
set_property(TARGET comparispawn APPEND PROPERTY INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(comparispawn cx_lib)

add_executable(cswitch cswitch.c)

add_executable(spawn spawn.c)
add_test(NAME spawn_not_not_test COMMAND spawn ! !)
add_test(NAME spawn_spawn_not_not_test COMMAND spawn "$<TARGET_FILE:spawn>" ! !)
add_test(NAME spawn_not_spawn_not_test COMMAND spawn ! "$<TARGET_FILE:spawn>" !)
add_test(NAME spawn_not_spawn_nothing_test COMMAND spawn ! "$<TARGET_FILE:spawn>")
