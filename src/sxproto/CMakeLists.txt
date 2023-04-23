
add_library(sxproto_lib_obj OBJECT
  "sxproto2textproto.c"
  "${PROJECT_SOURCE_DIR}/include/fildesh/sxproto.h"
)
add_library(sxproto_lib STATIC
  $<TARGET_OBJECTS:sxproto_lib_obj>
)
set_target_properties(sxproto_lib PROPERTIES OUTPUT_NAME "sxproto")
set(SXPROTO_LIB_PUBLIC_HEADERS
  "${PROJECT_SOURCE_DIR}/include/fildesh/sxproto.h"
)
set_target_properties(sxproto_lib PROPERTIES
  PUBLIC_HEADER "${SXPROTO_LIB_PUBLIC_HEADERS}")


install(TARGETS sxproto_lib
  COMPONENT "lib"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/fildesh"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fildesh")
