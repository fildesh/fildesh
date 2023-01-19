
add_executable(lib_array_test "array_test.c")
target_link_libraries(lib_array_test fildesh_lib)
add_test(NAME lib_array_test COMMAND lib_array_test)

add_executable(lib_kv_rbtree_test
  "kv_rbtree_test.c"
  "kv_rbtree_validation.c"
  "kv_rbtree_validation.h"
)
target_link_libraries(lib_kv_rbtree_test fildesh_lib)
add_test(NAME lib_kv_rbtree_test COMMAND lib_kv_rbtree_test)
