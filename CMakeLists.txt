
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
#   chatty.c
    )

include (${CxPath}/include.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinPath})

addbinexe (lace)

get_filename_component (d ${BinPath} ABSOLUTE)
set_property(SOURCE ${BldPath}/lace.c
    APPEND PROPERTY COMPILE_DEFINITIONS UtilBin="${d}")

addbinexe (add)
addbinexe (best-match)
addbinexe (xpipe)
addbinexe (void)
addbinexe (cat1)
addbinexe (ssh-all)
addbinexe (ujoin)
addbinexe (godo)
addbinexe (waitdo)
addbinexe (execfd)
addbinexe (echo)

#add_executable (chatty ${BldPath}/chatty.c ${CxFullCFiles})
#target_link_libraries (chatty rt)

