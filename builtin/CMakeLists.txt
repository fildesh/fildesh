
add_executable(add "add.c")
target_link_libraries(add lace_lib)

add_executable(cmp "cmp.c")
target_link_libraries(cmp lace_lib)

add_executable(elastic_pthread "elastic_pthread.c"
  "${CMAKE_SOURCE_DIR}/include/lace_posix_thread.h")
target_link_libraries(elastic_pthread lace_lib ${CMAKE_THREAD_LIBS_INIT})

add_executable(seq "seq.c")
target_link_libraries(seq cx_lib)

add_executable(time2sec "time2sec.c")
target_link_libraries(time2sec cx_lib)

add_executable(void "void.c")
target_link_libraries(void cx_lib)

add_executable(zec "zec.c")
target_link_libraries(zec lace_lib)
