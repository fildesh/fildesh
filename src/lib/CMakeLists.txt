
add_library(fildesh_lib_obj OBJECT
  "alloc.c"
  "array.c"
  "infile.c"
  "instream.c"
  "kve.c"
  "kve.h"
  "kv_single_list.c"
  "log.c"
  "mascii.c"
  "mascii.h"
  "outfile.c"
  "outstream.c"
  "string.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh/fildesh.h"
  )
add_library(fildesh_lib STATIC
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
  )
target_link_libraries(fildesh_lib ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(fildesh_lib PROPERTIES OUTPUT_NAME "fildesh")
set_target_properties(fildesh_lib PROPERTIES
  PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/fildesh/fildesh.h")

install(TARGETS fildesh_lib
  COMPONENT "lib"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/fildesh"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fildesh")
