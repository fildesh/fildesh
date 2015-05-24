
cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (lace)

set (CxPath ${CMAKE_CURRENT_SOURCE_DIR}/../dep/cx/src)
set (BinPath ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set (BldPath lace)

list (APPEND CFiles
  lace.c
  add.c
  best-match.c
  xpipe.c
  void.c
  cat1.c
  ssh-all.c
  ujoin.c
  godo.c
  waitdo.c
  execfd.c
  echo.c
  time2sec.c
  transpose.c
  chatty.c
  )

include (${CxPath}/include.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinPath})

addbinexe (lace lace.c)

get_filename_component (d ${BinPath} ABSOLUTE)

#add_definitions (-DUtilBin="${d}")
## The following causes CMake to segfault.
#set_property(SOURCE ${BldPath}/lace.c
#  APPEND PROPERTY COMPILE_DEFINITIONS UtilBin="${d}")
set_property(TARGET lace
  APPEND PROPERTY COMPILE_DEFINITIONS UtilBin="${d}")

addbinexe (add add.c)
addbinexe (best-match best-match.c)
addbinexe (xpipe xpipe.c)
addbinexe (void void.c)
addbinexe (cat1 cat1.c)
addbinexe (ssh-all ssh-all.c)
addbinexe (ujoin ujoin.c)
addbinexe (godo godo.c)
addbinexe (waitdo waitdo.c)
addbinexe (execfd execfd.c)
addbinexe (echo echo.c)
addbinexe (time2sec time2sec.c)
addbinexe (transpose transpose.c)

if (UNIX)
  addbinexe (chatty chatty.c)
  target_link_libraries (chatty rt)
  set_property (TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)
endif ()

