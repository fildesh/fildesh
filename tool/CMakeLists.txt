add_executable(cembed cembed.c)
add_executable(cswitch cswitch.c)
add_executable(expectish expectish.c)
add_executable(shout shout.c)
add_executable(shout_stdout "shout.c")
add_executable(spawn spawn.c)

add_library(fildesh_tool_lib STATIC
  "pipem.c"
  "${PROJECT_SOURCE_DIR}/include/fildesh_posix_thread.h"
  "${PROJECT_SOURCE_DIR}/include/fildesh_tool.h"
  )
target_link_libraries(fildesh_tool_lib fildesh_compat_lib ${CMAKE_THREAD_LIBS_INIT})

add_executable(comparispawn
  "comparispawn.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/cmptxt.c"
)
set_property(TARGET comparispawn
  APPEND PROPERTY COMPILE_DEFINITIONS "FILDESH_BUILTIN_LIBRARY")
target_link_libraries(comparispawn ${Fildesh_LIBRARIES} fildesh_tool_lib)

add_executable(fildespawn fildespawn.c)
target_link_libraries(fildespawn fildesh_compat_lib)
