
add_executable(fuzz_grow_mpop_fuzz_test
  "grow_mpop_fuzz_test.c"
  "fuzz_main.c")
target_link_libraries(fuzz_grow_mpop_fuzz_test fildesh_lib)
add_test(NAME fuzz_grow_mpop_fuzz_test COMMAND fuzz_grow_mpop_fuzz_test)

add_executable(fuzz_mascii_fuzz_test
  "mascii_fuzz_test.c"
  "fuzz_main.c")
target_link_libraries(fuzz_mascii_fuzz_test fildesh_lib)
add_test(NAME fuzz_mascii_fuzz_test COMMAND fuzz_mascii_fuzz_test)

add_executable(fuzz_parse_fildesh_line_fuzz_test
  "parse_fildesh_line_fuzz_test.c"
  "fuzz_main.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/defstr.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/defstr.h"
  "${CMAKE_SOURCE_DIR}/src/syntax/line.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/line.h"
  "${CMAKE_SOURCE_DIR}/src/syntax/symval.c"
  "${CMAKE_SOURCE_DIR}/src/syntax/symval.h"
)
target_link_libraries(fuzz_parse_fildesh_line_fuzz_test fildesh_lib)
add_test(NAME fuzz_parse_fildesh_line_fuzz_test COMMAND fuzz_parse_fildesh_line_fuzz_test)

add_subdirectory(kv)
