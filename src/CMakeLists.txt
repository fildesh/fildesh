
add_subdirectory(lib)
add_subdirectory(builtin)


# Conditionally referenced.
set(FILDESH_AIO_LIBS)
set(FILDESH_POLL_LIBS)
list(APPEND FILDESH_AIO_LIBS "rt")
if (APPLE)
  list(APPEND FILDESH_POLL_LIBS "c")
endif()

# Always referenced.
set(FILDESH_EXTRA_ELASTIC_DEFS)
set(FILDESH_EXTRA_ELASTIC_SOURCES)
set(FILDESH_EXTRA_ELASTIC_LIBS)

if (FILDESH_PREFER_PTHREAD)
  list(APPEND FILDESH_EXTRA_ELASTIC_DEFS "FILDESH_PREFER_PTHREAD")
endif()
if (FILDESH_PREFER_AIO)
  set(FILDESH_BUILTIN_ELASTIC_AIO_ON TRUE)
  list(APPEND FILDESH_EXTRA_ELASTIC_DEFS "FILDESH_PREFER_AIO")
endif()
if (FILDESH_PREFER_POLL)
  set(FILDESH_BUILTIN_ELASTIC_POLL_ON TRUE)
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
  "xpipe.c"
  "${CMAKE_SOURCE_DIR}/src/bin/version.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_posix_thread.h"
  "${CMAKE_SOURCE_DIR}/src/builtin/add.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/bestmatch.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/cmp.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/delimend.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/elastic_pthread.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/expect_failure.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/replace_string.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/seq.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/sponge.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/time2sec.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/transpose.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/ujoin.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/void.c"
  "${CMAKE_SOURCE_DIR}/src/builtin/zec.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/defstr.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/defstr.h"
  "${CMAKE_SOURCE_DIR}/src/syntax/line.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/line.h"
  "${CMAKE_SOURCE_DIR}/src/syntax/opt.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/opt.h"
  "${CMAKE_SOURCE_DIR}/src/syntax/symval.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/symval.h"
  )

set_property(TARGET fildesh_builtin_lib_obj APPEND PROPERTY COMPILE_DEFINITIONS
  FILDESH_BUILTIN_LIBRARY ${FILDESH_EXTRA_ELASTIC_DEFS})

add_library(fildesh_builtin_lib STATIC
  $<TARGET_OBJECTS:fildesh_builtin_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
  )
target_link_libraries(fildesh_builtin_lib
  ${CMAKE_THREAD_LIBS_INIT}
  ${FILDESH_EXTRA_ELASTIC_LIBS})

add_subdirectory(bin)

add_executable(execfd "execfd.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h")
target_link_libraries(execfd fildesh_lib)

add_executable(godo "godo.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h")
target_link_libraries(godo fildesh_lib)

add_executable(sshall "ssh-all.c")
target_link_libraries(sshall fildesh_lib)

add_executable(waitdo "waitdo.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h")
target_link_libraries(waitdo fildesh_lib)

if (FILDESH_BUILTIN_ELASTIC_AIO_ON)
  add_executable(elastic_aio "elastic_aio.c")
  target_link_libraries (elastic_aio fildesh_lib ${FILDESH_AIO_LIBS})
  set_property (TARGET elastic_aio
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)

  add_executable(chatty "chatty.c")
  target_link_libraries(chatty fildesh_lib ${FILDESH_AIO_LIBS})
  set_property(TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=201112L)
endif()

if (FILDESH_BUILTIN_ELASTIC_POLL_ON)
  add_executable(elastic_poll "elastic_poll.c")
  target_link_libraries (elastic_poll fildesh_lib)
endif()
