
add_library(cx_lib_obj OBJECT
  "alphatab.c"
  "alphatab.h"
  "def.h"
  "synhax.h"
  "syscx.c"
  "syscx.h"
  "table.h"
)
add_library(cx_lib STATIC
  $<TARGET_OBJECTS:cx_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
)
target_link_libraries(cx_lib ${CMAKE_THREAD_LIBS_INIT})

add_executable(table_test
  "table_test.c")
target_link_libraries(table_test cx_lib)
add_test(NAME table_test COMMAND table_test)
