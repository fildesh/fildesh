
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
  "${PROJECT_SOURCE_DIR}/src/syntax/defstr.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/defstr.h"
  "${PROJECT_SOURCE_DIR}/src/syntax/line.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/line.h"
  "${PROJECT_SOURCE_DIR}/src/syntax/symval.c"
  "${PROJECT_SOURCE_DIR}/src/syntax/symval.h"
)
target_link_libraries(fuzz_parse_fildesh_line_fuzz_test fildesh_lib)
add_test(NAME fuzz_parse_fildesh_line_fuzz_test COMMAND fuzz_parse_fildesh_line_fuzz_test)

add_executable(fuzz_parse_sxpb_fuzz_test
  "parse_sxpb_fuzz_test.c"
  "fuzz_main.c"
)
target_link_libraries(fuzz_parse_sxpb_fuzz_test ${FildeshSxproto_LIBRARIES})
add_test(NAME fuzz_parse_sxpb_fuzz_test COMMAND fuzz_parse_sxpb_fuzz_test)

add_executable(fuzz_parse_sxpb_smoke_test
  "parse_sxpb_fuzz_test.c"
  "parse_sxpb_smoke_test.c"
)
set_property(TARGET fuzz_parse_sxpb_smoke_test APPEND PROPERTY COMPILE_DEFINITIONS
  "SMOKE_TEST_ON")
target_link_libraries(fuzz_parse_sxpb_smoke_test ${FildeshSxproto_LIBRARIES})
add_test(NAME fuzz_parse_sxpb_smoke_test COMMAND fuzz_parse_sxpb_smoke_test)

add_subdirectory(kv)
