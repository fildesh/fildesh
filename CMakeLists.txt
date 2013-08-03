
cmake_minimum_required (VERSION 2.8)

project (lace)

set (CxPath ../cx)
set (BinPath ../bin)
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
  transpose.c
  #   chatty.c
  )

include (${CxPath}/include.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinPath})

addbinexe (lace lace.c)

get_filename_component (d ${BinPath} ABSOLUTE)
set_property(SOURCE ${BldPath}/lace.c
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
addbinexe (transpose transpose.c)

#add_executable (chatty ${BldPath}/chatty.c ${CxFullCFiles})
#target_link_libraries (chatty rt)

