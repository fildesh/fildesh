
add_library(obsolete_cx_lib_obj OBJECT
  "associa.h"
  "bittable.h"
  "bstree.c"
  "bstree.h"
  "lgtable.h"
  "rbtree.c"
  "rbtree.h"
  "table.h"
)
target_include_directories(obsolete_cx_lib_obj PRIVATE "${CMAKE_SOURCE_DIR}")
add_library(obsolete_cx_lib STATIC
  $<TARGET_OBJECTS:obsolete_cx_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
)
target_link_libraries(obsolete_cx_lib ${CMAKE_THREAD_LIBS_INIT})

add_library(cx_int_int_map_lib_obj OBJECT
  "int_int_map.c"
)
target_include_directories(cx_int_int_map_lib_obj PRIVATE "${CMAKE_SOURCE_DIR}")
add_library(cx_int_int_map_lib STATIC
  $<TARGET_OBJECTS:obsolete_cx_lib_obj>
  $<TARGET_OBJECTS:cx_int_int_map_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
)

add_executable(cx_bittable_test
  "bittable_test.c")
target_link_libraries(cx_bittable_test obsolete_cx_lib)
target_include_directories(cx_bittable_test PRIVATE "${CMAKE_SOURCE_DIR}")
add_test(NAME cx_bittable_test COMMAND cx_bittable_test)

add_executable(cx_lgtable_test
  "lgtable_test.c")
target_link_libraries(cx_lgtable_test obsolete_cx_lib)
target_include_directories(cx_lgtable_test PRIVATE "${CMAKE_SOURCE_DIR}")
add_test(NAME cx_lgtable_test COMMAND cx_lgtable_test)

add_executable(cx_rbtree_test
  "rbtree_test.c")
target_link_libraries(cx_rbtree_test obsolete_cx_lib)
target_include_directories(cx_rbtree_test PRIVATE "${CMAKE_SOURCE_DIR}")
add_test(NAME cx_rbtree_test COMMAND cx_rbtree_test)

add_executable(cx_table_test
  "table_test.c")
target_link_libraries(cx_table_test obsolete_cx_lib)
target_include_directories(cx_table_test PRIVATE "${CMAKE_SOURCE_DIR}")
add_test(NAME cx_table_test COMMAND cx_table_test)
