add_library(fildesh_compat_lib_obj OBJECT
  "errno.c"
  "env.c"
  "fd.c"
  "fd_exclusive.h"
  "file.c"
  "kill.c"
  "random.c"
  "sh.c"
  "string.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_compat_errno.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_compat_fd.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_compat_file.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_compat_random.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_compat_sh.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_compat_string.h"
  )
add_library(fildesh_compat_lib STATIC
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>)
target_link_libraries(fildesh_compat_lib ${CMAKE_THREAD_LIBS_INIT})
