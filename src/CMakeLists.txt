
cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (lace)

set (TopPath ${CMAKE_CURRENT_SOURCE_DIR}/..)
set (BinPath ${TopPath}/bin)
set (DepPath ${TopPath}/dep)
set (CxTopPath ${DepPath}/cx)
set (BldPath lace)

set (LaceCFiles
  lace.c
  add.c
  best-match.c
  xpipe.c
  void.c
  zec.c
  elastic.c
  ssh-all.c
  ujoin.c
  godo.c
  waitdo.c
  execfd.c
  time2sec.c
  transpose.c
  )

set (CFiles
  ${LaceCFiles}
  chatty.c
  )

list (APPEND HFiles
  utilace.h)

include (${CxTopPath}/src/include.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinPath})

addbinexe (lace ${LaceCFiles})
set (lace_exe "$<TARGET_FILE:lace>")

set_property(TARGET lace
  APPEND PROPERTY COMPILE_DEFINITIONS MAIN_LACE_EXECUTABLE)
set_property (TARGET lace
  APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)
target_link_libraries (lace rt)

addbinexe (add add.c)
addbinexe (best-match best-match.c)
addbinexe (xpipe xpipe.c)
addbinexe (void void.c)
addbinexe (zec zec.c)
addbinexe (ssh-all ssh-all.c)
addbinexe (ujoin ujoin.c)
addbinexe (godo godo.c)
addbinexe (waitdo waitdo.c)
addbinexe (execfd execfd.c)
addbinexe (time2sec time2sec.c)
addbinexe (transpose transpose.c)

set (zec_exe "$<TARGET_FILE:zec>")

if (UNIX)
  addbinexe (elastic elastic.c)
  target_link_libraries (elastic rt)
  set_property (TARGET elastic
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)
endif ()


if (UNIX)
  addbinexe (chatty chatty.c)
  target_link_libraries (chatty rt)
  set_property (TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=201112L)
endif ()


enable_testing ()
set (TestPath ${TopPath}/test)

## Ensure that the example we distribute actually works.
foreach (f hello test args silly name familiar cycle convoluted)
  add_test (NAME example_${f}
    WORKING_DIRECTORY ${TopPath}
    COMMAND
    comparispawn ${TestPath}/expect/${f}.txt
    ${lace_exe} -x example/${f}.lace
    )
endforeach ()

add_test (NAME example_args2
  WORKING_DIRECTORY ${TopPath}
  COMMAND
  comparispawn ${TestPath}/expect/args2.txt
  ${lace_exe} -x example/args.lace 2 7
  )

add_test (NAME cli_hello
  COMMAND
  comparispawn ${TestPath}/expect/hello.txt
  ${lace_exe} -stdout - -- "|< ${zec_exe} / 'hello'"
  )

