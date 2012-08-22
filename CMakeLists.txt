
cmake_minimum_required (VERSION 2.8)

project (eva)

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
#   chatty.c
    )

include (${CxPath}/include.cmake)

set (RUNTIME_OUTPUT_DIRECTORY ${BinPath})

add_executable (lace ${BldPath}/lace.c ${CxFullCFiles})
get_filename_component (d ${BinPath} ABSOLUTE)
set_property(SOURCE ${BldPath}/lace.c
    APPEND PROPERTY COMPILE_DEFINITIONS UtilBin="\\"${d}\\"")

add_executable (add ${BldPath}/add.c ${CxFullCFiles})
add_executable (best-match ${BldPath}/best-match.c ${CxFullCFiles})
add_executable (xpipe ${BldPath}/xpipe.c ${CxFullCFiles})
add_executable (void ${BldPath}/void.c ${CxFullCFiles})
add_executable (cat1 ${BldPath}/cat1.c ${CxFullCFiles})
add_executable (ssh-all ${BldPath}/ssh-all.c ${CxFullCFiles})
add_executable (ujoin ${BldPath}/ujoin.c ${CxFullCFiles})
add_executable (godo ${BldPath}/godo.c ${CxFullCFiles})
add_executable (waitdo ${BldPath}/waitdo.c ${CxFullCFiles})

#add_executable (chatty ${BldPath}/chatty.c ${CxFullCFiles})
#target_link_libraries (chatty rt)

