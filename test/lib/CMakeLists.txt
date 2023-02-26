
add_subdirectory(kv)

add_executable(lib_alloc_test
  "alloc_test.c")
target_link_libraries(lib_alloc_test fildesh_lib)
add_test(NAME lib_alloc_test COMMAND lib_alloc_test)

add_executable(lib_array_test
  "array_test.c")
target_link_libraries(lib_array_test fildesh_lib)
add_test(NAME lib_array_test COMMAND lib_array_test)

add_executable(lib_mascii_test
  "mascii_test.c")
target_link_libraries(lib_mascii_test fildesh_lib)
add_test(NAME lib_mascii_test COMMAND lib_mascii_test)

