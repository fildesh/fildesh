
find_package(benchmark)

if (NOT benchmark_FOUND)
  return()
endif()

set(CMAKE_BUILD_TYPE Release)

if (CMAKE_C_COMPILER_ID STREQUAL MSVC_COMPILER_ID)
elseif (CMAKE_C_COMPILER_ID STREQUAL TCC_COMPILER_ID)
  return()
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
  set(CMAKE_CXX_FLAGS "-march=native")
endif()

set(FILDESH_BENCHMARK_TEST_ARG "--benchmark_min_time=0.000001")


add_executable(benchmark_file_write_read_benchmark
  "file_write_read_benchmark.cc")
target_link_libraries(benchmark_file_write_read_benchmark fildesh_lib benchmark::benchmark)
add_test(NAME benchmark_file_write_read_benchmark COMMAND
  "${CMAKE_COMMAND}" -E env "TEST_TMPDIR=${CMAKE_CURRENT_BINARY_DIR}"
  "$<TARGET_FILE:benchmark_file_write_read_benchmark>" "${FILDESH_BENCHMARK_TEST_ARG}")

add_executable(benchmark_grow_mpop_benchmark
  "grow_mpop_benchmark.cc")
target_link_libraries(benchmark_grow_mpop_benchmark fildesh_lib benchmark::benchmark)
add_test(NAME benchmark_grow_mpop_benchmark COMMAND
  benchmark_grow_mpop_benchmark "${FILDESH_BENCHMARK_TEST_ARG}")

add_executable(benchmark_map_benchmark
  "map_benchmark.cc")
target_link_libraries(benchmark_map_benchmark cx_int_int_map_lib benchmark::benchmark)
add_test(NAME benchmark_map_benchmark COMMAND
  benchmark_map_benchmark "${FILDESH_BENCHMARK_TEST_ARG}")


add_executable(benchmark_strchr_benchmark
  "strchr_benchmark.cc"
  "${CMAKE_SOURCE_DIR}/src/lib/mascii.c"
  "${CMAKE_SOURCE_DIR}/src/lib/mascii.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh/fildesh.h")
target_include_directories(benchmark_strchr_benchmark PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(benchmark_strchr_benchmark benchmark::benchmark)
add_test(NAME benchmark_strchr_benchmark COMMAND
  benchmark_strchr_benchmark "${FILDESH_BENCHMARK_TEST_ARG}")

add_executable(benchmark_strcspn_benchmark
  "strcspn_benchmark.cc"
  "${CMAKE_SOURCE_DIR}/src/lib/mascii.c"
  "${CMAKE_SOURCE_DIR}/src/lib/mascii.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh/fildesh.h")
target_include_directories(benchmark_strcspn_benchmark PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(benchmark_strcspn_benchmark benchmark::benchmark)
add_test(NAME benchmark_strcspn_benchmark COMMAND
  benchmark_strcspn_benchmark "${FILDESH_BENCHMARK_TEST_ARG}")

add_executable(benchmark_strdup_benchmark
  "strdup_benchmark.cc")
target_link_libraries(benchmark_strdup_benchmark fildesh_lib benchmark::benchmark)
add_test(NAME benchmark_strdup_benchmark COMMAND
  benchmark_strdup_benchmark "${FILDESH_BENCHMARK_TEST_ARG}")
