
add_library(lace_lib STATIC
  "infile.c"
  "instream.c"
  "outfile.c"
  "outstream.c"
  "string.c"
  )


add_subdirectory(cx)

find_package(Threads REQUIRED)

if (WIN32)
  set (ELASTIC_SOURCE "elastic_pthread.c")
elseif (APPLE)
  set (ELASTIC_SOURCE "elastic_poll.c")
else()
  set (ELASTIC_SOURCE "elastic_aio.c")
endif()

set (AIO_LIB "rt")
if (APPLE)
  set (AIO_LIB "c")
endif()


add_executable(lace
  "lace.c"
  "utilace.h"
  "add.c"
  "best-match.c"
  "compatible_pthread.h"
  "${ELASTIC_SOURCE}"
  "execfd.c"
  "godo.c"
  "ssh-all.c"
  "time2sec.c"
  "transpose.c"
  "ujoin.c"
  "void.c"
  "waitdo.c"
  "xpipe.c"
  "zec.c"
  )
set_property(TARGET lace
  APPEND PROPERTY COMPILE_DEFINITIONS MAIN_LACE_EXECUTABLE)
target_link_libraries(lace cx_lib ${CMAKE_THREAD_LIBS_INIT})
if (UNIX AND NOT APPLE)
  target_link_libraries(lace ${AIO_LIB})
endif()



add_executable(add
  add.c)
target_link_libraries(add lace_lib)

add_executable(best-match
  best-match.c
  utilace.h)
target_link_libraries(best-match cx_lib)

add_executable(execfd
  execfd.c
  utilace.h)
target_link_libraries(execfd cx_lib)

add_executable(godo
  godo.c
  utilace.h)
target_link_libraries(godo cx_lib)

add_executable(ssh-all
  ssh-all.c
  utilace.h)
target_link_libraries(ssh-all cx_lib)

add_executable(time2sec
  time2sec.c
  utilace.h)
target_link_libraries(time2sec cx_lib)

add_executable(transpose
  transpose.c
  utilace.h)
target_link_libraries(transpose cx_lib)

add_executable(ujoin
  ujoin.c
  utilace.h)
target_link_libraries(ujoin cx_lib)

add_executable(void
  void.c
  utilace.h)
target_link_libraries(void cx_lib)

add_executable(waitdo
  waitdo.c
  utilace.h)
target_link_libraries(waitdo cx_lib)

add_executable(xpipe
  xpipe.c
  utilace.h)
target_link_libraries(xpipe cx_lib)

add_executable(zec
  zec.c)
target_link_libraries(zec lace_lib)


add_executable(elastic_pthread "elastic_pthread.c" "compatible_pthread.h")
target_link_libraries(elastic_pthread lace_lib ${CMAKE_THREAD_LIBS_INIT})

if (UNIX)
  add_executable(elastic_poll elastic_poll.c utilace.h)
  target_link_libraries (elastic_poll cx_lib)
endif ()

if (UNIX AND NOT APPLE)
  add_executable(elastic_aio elastic_aio.c utilace.h)
  target_link_libraries (elastic_aio cx_lib ${AIO_LIB})
  set_property (TARGET elastic_aio
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)

  add_executable(chatty
    chatty.c)
  target_link_libraries (chatty cx_lib ${AIO_LIB})
  set_property (TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=201112L)
endif ()


install(TARGETS lace
  RUNTIME DESTINATION bin)
