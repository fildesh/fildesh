
add_library(sxproto_lib_obj OBJECT
  "sxproto2textproto.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh/sxproto.h"
)
add_library(sxproto_lib STATIC
  $<TARGET_OBJECTS:sxproto_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
)
target_link_libraries(sxproto_lib ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(sxproto_lib PROPERTIES OUTPUT_NAME "sxproto")
set(SXPROTO_LIB_PUBLIC_HEADERS
  "${CMAKE_SOURCE_DIR}/include/fildesh/fildesh.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh/ifstream.hh"
  "${CMAKE_SOURCE_DIR}/include/fildesh/ofstream.hh"
)
set_target_properties(sxproto_lib PROPERTIES
  PUBLIC_HEADER "${SXPROTO_LIB_PUBLIC_HEADERS}")


add_executable(sxproto2textproto "sxproto2textproto_main.c")
target_link_libraries(sxproto2textproto sxproto_lib)


install(TARGETS sxproto_lib
  COMPONENT "lib"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/fildesh"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fildesh")

install(TARGETS sxproto2textproto
  COMPONENT "bin"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
