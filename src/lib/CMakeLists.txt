
add_library(fildesh_lib_obj OBJECT
  "alloc.c"
  "array.c"
  "infile.c"
  "instream.c"
  "kve.c"
  "kve.h"
  "kv.c"
  "kv.h"
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
set(FILDESH_LIB_PUBLIC_HEADERS
  "${CMAKE_SOURCE_DIR}/include/fildesh/fildesh.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh/ifstream.hh"
  "${CMAKE_SOURCE_DIR}/include/fildesh/ofstream.hh"
  )
set_target_properties(fildesh_lib PROPERTIES
  PUBLIC_HEADER "${FILDESH_LIB_PUBLIC_HEADERS}")

install(TARGETS fildesh_lib
  COMPONENT "lib"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/fildesh"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fildesh")
