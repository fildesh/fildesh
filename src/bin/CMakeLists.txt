
add_executable(fildesh "main.c")
target_link_libraries(fildesh fildesh_builtin_lib)

install(TARGETS fildesh
  COMPONENT "bin"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
