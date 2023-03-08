
add_subdirectory(compat)
add_subdirectory(lib)
add_subdirectory(cc)
add_subdirectory(tool)
add_subdirectory(src)
add_subdirectory(syntax)

set (fildesh_exe "$<TARGET_FILE:fildesh>")
set (zec_exe "$<TARGET_FILE:zec>")

add_subdirectory(benchmark)
add_subdirectory(builtin)
add_subdirectory(example)
add_subdirectory(fault)
add_subdirectory(flag)
add_subdirectory(fuzz)
add_subdirectory(sxproto)
