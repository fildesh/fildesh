
add_library(kv_validation_lib_obj OBJECT
  "brbtree_validation.c"
  "brbtree_validation.h"
  "rbtree_validation.c"
  "rbtree_validation.h"
  )

add_library(kv_validation_lib STATIC
  $<TARGET_OBJECTS:kv_validation_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
  )
target_link_libraries(kv_validation_lib
  ${CMAKE_THREAD_LIBS_INIT})

add_executable(lib_kv_kve_test
  "kve_test.c")
target_link_libraries(lib_kv_kve_test fildesh_lib)
add_test(NAME lib_kv_kve_test COMMAND lib_kv_kve_test)

add_executable(lib_kv_rbtree_test
  "rbtree_test.c")
target_link_libraries(lib_kv_rbtree_test kv_validation_lib)
add_test(NAME lib_kv_rbtree_test COMMAND lib_kv_rbtree_test)

add_executable(lib_kv_single_list_test
  "single_list_test.c")
target_link_libraries(lib_kv_single_list_test fildesh_lib)
add_test(NAME lib_kv_single_list_test COMMAND lib_kv_single_list_test)
