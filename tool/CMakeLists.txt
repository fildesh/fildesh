add_executable(cembed cembed.c)
add_executable(cswitch cswitch.c)
add_executable(expectish expectish.c)
add_executable(shout shout.c)
add_executable(shout_stdout "shout.c")
add_executable(spawn spawn.c)

add_library(fildesh_tool_lib STATIC
  "pipem.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_posix_thread.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_tool.h"
  )
set_property(TARGET fildesh_tool_lib
  APPEND PROPERTY COMPILE_DEFINITIONS FILDESH_TOOL_LIBRARY)
target_link_libraries(fildesh_tool_lib fildesh_compat_lib ${CMAKE_THREAD_LIBS_INIT})

add_executable(comparispawn comparispawn.c)
target_link_libraries(comparispawn fildesh_lib fildesh_tool_lib)

add_executable(fildespawn fildespawn.c)
target_link_libraries(fildespawn fildesh_compat_lib)
