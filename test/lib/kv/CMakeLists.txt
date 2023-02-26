
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
