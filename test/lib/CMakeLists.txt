
add_executable(lib_array_test "array_test.c")
target_link_libraries(lib_array_test fildesh_lib)
add_test(NAME lib_array_test COMMAND lib_array_test)
