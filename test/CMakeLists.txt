
set(FildeshTool_shout_EXECUTABLE "$<TARGET_FILE:shout>")
set(FildeshTool_spawn_EXECUTABLE "$<TARGET_FILE:spawn>")

# Sorted by dependence.
add_subdirectory(compat)
add_subdirectory(tool)
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(sxproto)
add_subdirectory(cc)
add_subdirectory(fuzz)

# Sorted by name.
add_subdirectory(benchmark)
add_subdirectory(builtin)
add_subdirectory(example)
add_subdirectory(fault)
add_subdirectory(flag)
add_subdirectory(syntax)
