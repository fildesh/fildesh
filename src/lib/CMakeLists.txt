
add_library(fildesh_lib_obj OBJECT
  "alloc.c"
  "array.c"
  "infile.c"
  "instream.c"
  "log.c"
  "mascii.c"
  "mascii.h"
  "outfile.c"
  "outstream.c"
  "string.c"
  "kv/brbtree.c"
  "kv/bstree.c"
  "kv/bstree.h"
  "kv/kv.c"
  "kv/kv.h"
  "kv/kve.c"
  "kv/kve.h"
  "kv/rbtree.c"
  "kv/single_list.c"
  "${PROJECT_SOURCE_DIR}/include/fildesh/fildesh.h"
  )
add_library(fildesh_lib STATIC
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
  )
target_link_libraries(fildesh_lib ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(fildesh_lib PROPERTIES OUTPUT_NAME "fildesh")
set(FILDESH_LIB_PUBLIC_HEADERS
  "${PROJECT_SOURCE_DIR}/include/fildesh/fildesh.h"
  "${PROJECT_SOURCE_DIR}/include/fildesh/ifstream.hh"
  "${PROJECT_SOURCE_DIR}/include/fildesh/ofstream.hh"
  "${PROJECT_SOURCE_DIR}/include/fildesh/string.hh"
  )
set_target_properties(fildesh_lib PROPERTIES
  PUBLIC_HEADER "${FILDESH_LIB_PUBLIC_HEADERS}")

install(TARGETS fildesh_lib
  COMPONENT "lib"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/fildesh"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fildesh")
