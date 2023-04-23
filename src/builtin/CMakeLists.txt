
if (NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  # Avoid polluting namespace of dependent project.
  return()
endif()

add_executable(add "add.c")
target_link_libraries(add fildesh_lib)

add_executable(bestmatch "bestmatch.c")
target_link_libraries(bestmatch fildesh_lib)

add_executable(cmp "cmp.c")
target_link_libraries(cmp fildesh_lib)

add_executable(elastic_pthread "elastic_pthread.c"
  "${PROJECT_SOURCE_DIR}/include/fildesh_posix_thread.h")
target_link_libraries(elastic_pthread fildesh_lib ${CMAKE_THREAD_LIBS_INIT})

add_executable(expect_failure "expect_failure.c")
target_link_libraries(expect_failure fildesh_lib)

add_executable(replace_string "replace_string.c")
target_link_libraries(replace_string fildesh_lib)

add_executable(seq "seq.c")
target_link_libraries(seq fildesh_lib)

add_executable(sponge "sponge.c")
target_link_libraries(sponge fildesh_lib)

add_executable(sxproto2textproto "sxproto2textproto.c")
target_link_libraries(sxproto2textproto sxproto_lib fildesh_lib)

add_executable(time2sec "time2sec.c")
target_link_libraries(time2sec fildesh_lib)

add_executable(transpose "transpose.c")
target_link_libraries(transpose fildesh_lib)

add_executable(ujoin "ujoin.c")
target_link_libraries(ujoin fildesh_lib)

add_executable(void "void.c")
target_link_libraries(void fildesh_lib)

add_executable(zec "zec.c")
target_link_libraries(zec fildesh_lib)
