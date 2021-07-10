add_executable(cembed cembed.c)
add_executable(cswitch cswitch.c)
add_executable(expectish expectish.c)
add_executable(shout shout.c)
add_executable(shout_stdout "shout.c")
add_executable(spawn spawn.c)

add_library(lace_tool_lib STATIC
  "pipem.c"
  "${CMAKE_SOURCE_DIR}/include/lace_posix_thread.h"
  "${CMAKE_SOURCE_DIR}/include/lace_tool.h"
  )
set_property(TARGET lace_tool_lib
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_TOOL_LIBRARY)
target_link_libraries(lace_tool_lib lace_compat_lib ${CMAKE_THREAD_LIBS_INIT})

add_executable(comparispawn comparispawn.c)
target_link_libraries(comparispawn lace_lib lace_tool_lib)
