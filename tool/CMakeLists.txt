add_executable(cembed cembed.c)

add_executable(comparispawn comparispawn.c)
set_property(TARGET comparispawn APPEND PROPERTY INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(comparispawn cx_lib)

add_executable(cswitch cswitch.c)
add_executable(expectish expectish.c)
add_executable(shout shout.c)
add_executable(spawn spawn.c)

add_library(lace_tool_lib STATIC
  "argspawn.c"
  "expectish.c"
  "pipem.c"
  "shout.c"
  "spawn.c"
  "${CMAKE_SOURCE_DIR}/include/lace_tool.h"
  )
set_property(TARGET lace_tool_lib
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_TOOL_LIBRARY)
target_link_libraries(lace_tool_lib lace_compat_lib ${CMAKE_THREAD_LIBS_INIT})
