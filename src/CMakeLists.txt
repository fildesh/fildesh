
add_subdirectory(lib)
add_subdirectory(builtin)
add_subdirectory(sxproto)


# Always referenced.
set(FILDESH_EXTRA_ELASTIC_DEFS)
set(FILDESH_EXTRA_ELASTIC_SOURCES)
set(FILDESH_EXTRA_ELASTIC_LIBS)

if (FILDESH_PREFER_PTHREAD)
  list(APPEND FILDESH_EXTRA_ELASTIC_DEFS "FILDESH_PREFER_PTHREAD")
endif()
if (FILDESH_PREFER_AIO)
  list(APPEND FILDESH_EXTRA_ELASTIC_DEFS "FILDESH_PREFER_AIO")
endif()
if (FILDESH_PREFER_POLL)
  list(APPEND FILDESH_EXTRA_ELASTIC_DEFS "FILDESH_PREFER_POLL")
endif()

if (FILDESH_BUILTIN_ELASTIC_AIO_ON)
  list(APPEND FILDESH_EXTRA_ELASTIC_DEFS "FILDESH_BUILTIN_ELASTIC_AIO_ON")
  list(APPEND FILDESH_EXTRA_ELASTIC_SOURCES "elastic_aio.c")
  list(APPEND FILDESH_EXTRA_ELASTIC_LIBS ${FILDESH_AIO_LIBS})
endif()
if (FILDESH_BUILTIN_ELASTIC_POLL_ON)
  list(APPEND FILDESH_EXTRA_ELASTIC_DEFS "FILDESH_BUILTIN_ELASTIC_POLL_ON")
  list(APPEND FILDESH_EXTRA_ELASTIC_SOURCES "elastic_poll.c")
  list(APPEND FILDESH_EXTRA_ELASTIC_LIBS ${FILDESH_POLL_LIBS})
endif()


add_library(fildesh_builtin_lib_obj OBJECT
  "fildesh.c"
  ${FILDESH_EXTRA_ELASTIC_SOURCES}
  "execfd.c"
  "godo.c"
  "ssh-all.c"
  "waitdo.c"
  "xargz.c"
  "xpipe.c"
  "${PROJECT_SOURCE_DIR}/src/bin/version.h"
  "${PROJECT_SOURCE_DIR}/include/fildesh_posix_thread.h"
  "${PROJECT_SOURCE_DIR}/src/builtin/add.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/bestmatch.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/cmp.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/delimend.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/elastic_pthread.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/expect_failure.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/fildesh_builtin.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/fildesh_builtin.h"
  "${PROJECT_SOURCE_DIR}/src/builtin/oargz.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/replace_string.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/seq.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/sponge.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/sxpb2json.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/sxpb2txtpb.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/time2sec.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/transpose.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/ujoin.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/void.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/zec.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/defstr.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/defstr.h"
  "${PROJECT_SOURCE_DIR}/src/syntax/line.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/line.h"
  "${PROJECT_SOURCE_DIR}/src/syntax/opt.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/opt.h"
  "${PROJECT_SOURCE_DIR}/src/syntax/symval.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/symval.h"
  )

set_property(TARGET fildesh_builtin_lib_obj APPEND PROPERTY COMPILE_DEFINITIONS
  FILDESH_BUILTIN_LIBRARY ${FILDESH_EXTRA_ELASTIC_DEFS})

add_library(fildesh_builtin_lib STATIC
  $<TARGET_OBJECTS:fildesh_builtin_lib_obj>
  $<TARGET_OBJECTS:sxproto_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
  )
target_link_libraries(fildesh_builtin_lib
  ${CMAKE_THREAD_LIBS_INIT}
  ${FILDESH_EXTRA_ELASTIC_LIBS})

add_subdirectory(bin)


if (NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  # Avoid polluting namespace of dependent project.
  return()
endif()

add_executable(execfd "execfd.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/fildesh_builtin.h")
target_link_libraries(execfd fildesh_lib)

add_executable(godo "godo.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/fildesh_builtin.h")
target_link_libraries(godo fildesh_lib)

add_executable(sshall "ssh-all.c")
target_link_libraries(sshall fildesh_lib)

add_executable(waitdo "waitdo.c"
  "${PROJECT_SOURCE_DIR}/src/builtin/fildesh_builtin.h")
target_link_libraries(waitdo fildesh_lib)

if (FILDESH_BUILTIN_ELASTIC_AIO_ON)
  add_executable(elastic_aio "elastic_aio.c")
  target_link_libraries (elastic_aio fildesh_lib ${FILDESH_AIO_LIBS})
  set_property (TARGET elastic_aio
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)
endif()

if (FILDESH_BUILTIN_ELASTIC_POLL_ON)
  add_executable(elastic_poll "elastic_poll.c")
  target_link_libraries (elastic_poll fildesh_lib)
endif()
