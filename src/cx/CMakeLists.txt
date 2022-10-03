
add_library(cx_lib_obj OBJECT
  "syscx.c"
  "syscx.h"
)
add_library(cx_lib STATIC
  $<TARGET_OBJECTS:cx_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
)
target_link_libraries(cx_lib ${CMAKE_THREAD_LIBS_INIT})
