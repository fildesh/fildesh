
add_subdirectory(cx)

add_executable(lace
  "lace.c"
  "utilace.h"
  "add.c"
  "best-match.c"
  "elastic.c"
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
set_property (TARGET lace
  APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)
target_link_libraries(lace cx_lib rt)


add_executable(add
  add.c
  utilace.h)
target_link_libraries(add cx_lib)

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
  zec.c
  utilace.h)
target_link_libraries(zec cx_lib)


if (UNIX)
  add_executable(elastic elastic.c utilace.h)
  target_link_libraries (elastic cx_lib rt)
  set_property (TARGET elastic
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)
endif ()


if (UNIX)
  add_executable(chatty
    chatty.c)
  target_link_libraries (chatty cx_lib rt)
  set_property (TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=201112L)
endif ()


add_executable(comparispawn
  comparispawn.c)
target_link_libraries(comparispawn cx_lib)


install(TARGETS lace)
