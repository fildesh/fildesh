
add_executable(fuzz_kv_ensure_fuzz_test
  "ensure_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c")
target_link_libraries(fuzz_kv_ensure_fuzz_test fildesh_lib)
add_test(NAME fuzz_kv_ensure_fuzz_test COMMAND fuzz_kv_ensure_fuzz_test)

add_executable(fuzz_kv_ensure_smoke_test
  "ensure_smoke_test.c"
  "ensure_fuzz_test.c")
target_link_libraries(fuzz_kv_ensure_smoke_test fildesh_lib)
add_test(NAME fuzz_kv_ensure_smoke_test COMMAND fuzz_kv_ensure_smoke_test)

add_executable(fuzz_kv_brbtree_smoke_test
  "brbtree_smoke_test.c"
  "brbtree_fuzz_test.c"
  "kv_fuzz_common.c"
  "kv_fuzz_common.h")
target_link_libraries(fuzz_kv_brbtree_smoke_test kv_validation_lib)
add_test(NAME fuzz_kv_brbtree_smoke_test COMMAND fuzz_kv_brbtree_smoke_test)

add_executable(fuzz_kv_bstree_fuzz_test
  "bstree_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c"
  "kv_fuzz_common.c"
  "kv_fuzz_common.h")
target_link_libraries(fuzz_kv_bstree_fuzz_test fildesh_lib)
add_test(NAME fuzz_kv_bstree_fuzz_test COMMAND fuzz_kv_bstree_fuzz_test)

add_executable(fuzz_kv_bstree_smoke_test
  "bstree_smoke_test.c"
  "bstree_fuzz_test.c"
  "kv_fuzz_common.c"
  "kv_fuzz_common.h")
target_link_libraries(fuzz_kv_bstree_smoke_test fildesh_lib)
add_test(NAME fuzz_kv_bstree_smoke_test COMMAND fuzz_kv_bstree_smoke_test)

add_executable(fuzz_kv_rbtree_fuzz_test
  "rbtree_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c"
  "kv_fuzz_common.c"
  "kv_fuzz_common.h"
)
target_link_libraries(fuzz_kv_rbtree_fuzz_test kv_validation_lib)
add_test(NAME fuzz_kv_rbtree_fuzz_test COMMAND fuzz_kv_rbtree_fuzz_test)

add_executable(fuzz_kv_rbtree_smoke_test
  "rbtree_smoke_test.c"
  "rbtree_fuzz_test.c"
  "kv_fuzz_common.c"
  "kv_fuzz_common.h"
)
target_link_libraries(fuzz_kv_rbtree_smoke_test kv_validation_lib)
add_test(NAME fuzz_kv_rbtree_smoke_test COMMAND fuzz_kv_rbtree_smoke_test)

add_executable(fuzz_kv_single_list_fuzz_test
  "single_list_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c"
  "kv_fuzz_common.c"
  "kv_fuzz_common.h")
target_link_libraries(fuzz_kv_single_list_fuzz_test fildesh_lib)
add_test(NAME fuzz_kv_single_list_fuzz_test COMMAND fuzz_kv_single_list_fuzz_test)

add_executable(fuzz_kv_single_list_smoke_test
  "single_list_smoke_test.c"
  "single_list_fuzz_test.c"
  "kv_fuzz_common.c"
  "kv_fuzz_common.h")
target_link_libraries(fuzz_kv_single_list_smoke_test fildesh_lib)
add_test(NAME fuzz_kv_single_list_smoke_test COMMAND fuzz_kv_single_list_smoke_test)

