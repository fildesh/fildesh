
add_library(fildesh_lib_obj OBJECT
  "alloc.c"
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
  "${CMAKE_SOURCE_DIR}/include/fildesh.h"
  )
add_library(fildesh_lib STATIC
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
  )
target_link_libraries(fildesh_lib ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(fildesh_lib PROPERTIES OUTPUT_NAME "fildesh")
set_target_properties(fildesh_lib PROPERTIES PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/fildesh.h")


add_subdirectory(cx)


set(FILDESH_EXTRA_ELASTIC_AIO_LIBS)
set(FILDESH_EXTRA_ELASTIC_SOURCES)
if (FILDESH_RELEASING)
  # No extra stuff during release.
elseif(UNIX)
  if (APPLE)
    list(APPEND FILDESH_EXTRA_ELASTIC_SOURCES "elastic_poll.c")
    list(APPEND FILDESH_EXTRA_ELASTIC_AIO_LIBS "c")
  else()
    list(APPEND FILDESH_EXTRA_ELASTIC_SOURCES "elastic_aio.c" "elastic_poll.c")
    list(APPEND FILDESH_EXTRA_ELASTIC_AIO_LIBS "rt")
  endif()
endif()

add_library(fildesh_builtin_lib_obj OBJECT
  "fildesh.c"
  ${FILDESH_EXTRA_ELASTIC_SOURCES}
  "execfd.c"
  "godo.c"
  "parse_fildesh.c"
  "parse_fildesh.h"
  "ssh-all.c"
  "waitdo.c"
  "xpipe.c"
  "${CMAKE_SOURCE_DIR}/builtin/add.c"
  "${CMAKE_SOURCE_DIR}/builtin/bestmatch.c"
  "${CMAKE_SOURCE_DIR}/builtin/cmp.c"
  "${CMAKE_SOURCE_DIR}/builtin/elastic_pthread.c"
  "${CMAKE_SOURCE_DIR}/builtin/expect_failure.c"
  "${CMAKE_SOURCE_DIR}/builtin/replace_string.c"
  "${CMAKE_SOURCE_DIR}/builtin/seq.c"
  "${CMAKE_SOURCE_DIR}/builtin/sponge.c"
  "${CMAKE_SOURCE_DIR}/builtin/time2sec.c"
  "${CMAKE_SOURCE_DIR}/builtin/transpose.c"
  "${CMAKE_SOURCE_DIR}/builtin/ujoin.c"
  "${CMAKE_SOURCE_DIR}/builtin/void.c"
  "${CMAKE_SOURCE_DIR}/builtin/zec.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh_posix_thread.h"
  )

set_property(TARGET fildesh_builtin_lib_obj APPEND PROPERTY COMPILE_DEFINITIONS
  FILDESH_BUILTIN_LIBRARY)

if (WIN32)
  # Nothing special.
elseif(FILDESH_RELEASING)
  # Nothing special.
elseif(APPLE)
  set_property(TARGET fildesh_builtin_lib_obj APPEND PROPERTY COMPILE_DEFINITIONS
    FILDESH_BUILTIN_PERMIT_ELASTIC_POLL)
else()
  set_property(TARGET fildesh_builtin_lib_obj
    APPEND PROPERTY COMPILE_DEFINITIONS
    FILDESH_BUILTIN_PERMIT_ELASTIC_AIO
    FILDESH_BUILTIN_PERMIT_ELASTIC_POLL)
endif()

add_library(fildesh_builtin_lib STATIC
  $<TARGET_OBJECTS:fildesh_builtin_lib_obj>
  $<TARGET_OBJECTS:cx_lib_obj>
  $<TARGET_OBJECTS:fildesh_lib_obj>
  $<TARGET_OBJECTS:fildesh_compat_lib_obj>
  )
if (FILDESH_RELEASING)
  target_link_libraries(fildesh_builtin_lib ${CMAKE_THREAD_LIBS_INIT})
else()
  target_link_libraries(fildesh_builtin_lib
    ${CMAKE_THREAD_LIBS_INIT}
    ${FILDESH_EXTRA_ELASTIC_AIO_LIBS})
endif()

add_executable(fildesh "main_fildesh.c")
target_link_libraries(fildesh fildesh_builtin_lib)


add_executable(execfd "execfd.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h")
target_link_libraries(execfd fildesh_lib)

add_executable(godo "godo.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h")
target_link_libraries(godo cx_lib)

add_executable(sshall "ssh-all.c")
target_link_libraries(sshall fildesh_lib)

add_executable(waitdo "waitdo.c"
  "${CMAKE_SOURCE_DIR}/include/fildesh_builtin.h")
target_link_libraries(waitdo cx_lib)

if (UNIX)
  add_executable(elastic_poll "elastic_poll.c")
  target_link_libraries (elastic_poll cx_lib)
endif()

if(FILDESH_RELEASING)
  # These don't need to be built during release.
elseif(UNIX AND NOT APPLE)
  add_executable(elastic_aio "elastic_aio.c")
  target_link_libraries (elastic_aio cx_lib ${FILDESH_EXTRA_ELASTIC_AIO_LIBS})
  set_property (TARGET elastic_aio
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)

  add_executable(chatty "chatty.c")
  target_link_libraries(chatty fildesh_lib ${FILDESH_EXTRA_ELASTIC_AIO_LIBS})
  set_property(TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=201112L)
endif()


include(GNUInstallDirs)
install(TARGETS fildesh
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(TARGETS fildesh_lib
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

