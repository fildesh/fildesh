
add_executable(fuzz_kv_ensure_fuzz_test
  "ensure_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.c"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.h"
)
target_link_libraries(fuzz_kv_ensure_fuzz_test fildesh_lib)
add_test(NAME fuzz_kv_ensure_fuzz_test COMMAND fuzz_kv_ensure_fuzz_test)

add_executable(fuzz_kv_ensure_smoke_test
  "ensure_smoke_test.c"
  "ensure_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.c"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.h"
)
target_link_libraries(fuzz_kv_ensure_smoke_test fildesh_lib)
add_test(NAME fuzz_kv_ensure_smoke_test COMMAND fuzz_kv_ensure_smoke_test)

add_executable(fuzz_kv_bstree_fuzz_test
  "bstree_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c"
  "fuzz_common.c"
  "fuzz_common.h")
target_link_libraries(fuzz_kv_bstree_fuzz_test fildesh_lib)
add_test(NAME fuzz_kv_bstree_fuzz_test COMMAND fuzz_kv_bstree_fuzz_test)

add_executable(fuzz_kv_bstree_smoke_test
  "bstree_smoke_test.c"
  "bstree_fuzz_test.c"
  "fuzz_common.c"
  "fuzz_common.h")
target_link_libraries(fuzz_kv_bstree_smoke_test fildesh_lib)
add_test(NAME fuzz_kv_bstree_smoke_test COMMAND fuzz_kv_bstree_smoke_test)


add_executable(fuzz_kv_rbtree_fuzz_test
  "rbtree_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c"
  "fuzz_common.c"
  "fuzz_common.h"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.c"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.h"
)
target_link_libraries(fuzz_kv_rbtree_fuzz_test fildesh_lib)
add_test(NAME fuzz_kv_rbtree_fuzz_test COMMAND fuzz_kv_rbtree_fuzz_test)

add_executable(fuzz_kv_rbtree_smoke_test
  "rbtree_smoke_test.c"
  "rbtree_fuzz_test.c"
  "fuzz_common.c"
  "fuzz_common.h"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.c"
  "${CMAKE_SOURCE_DIR}/test/lib/kv_rbtree_validation.h"
)
target_link_libraries(fuzz_kv_rbtree_smoke_test fildesh_lib)
add_test(NAME fuzz_kv_rbtree_smoke_test COMMAND fuzz_kv_rbtree_smoke_test)


add_executable(fuzz_kv_single_list_fuzz_test
  "single_list_fuzz_test.c"
  "${CMAKE_SOURCE_DIR}/test/fuzz/fuzz_main.c"
  "fuzz_common.c"
  "fuzz_common.h")
target_link_libraries(fuzz_kv_single_list_fuzz_test fildesh_lib)
add_test(NAME fuzz_kv_single_list_fuzz_test COMMAND fuzz_kv_single_list_fuzz_test)

add_executable(fuzz_kv_single_list_smoke_test
  "single_list_smoke_test.c"
  "single_list_fuzz_test.c"
  "fuzz_common.c"
  "fuzz_common.h")
target_link_libraries(fuzz_kv_single_list_smoke_test fildesh_lib)
add_test(NAME fuzz_kv_single_list_smoke_test COMMAND fuzz_kv_single_list_smoke_test)

