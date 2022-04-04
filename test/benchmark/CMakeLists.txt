
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

add_executable(strchr_benchmark "strchr_benchmark.cc"
  "${CMAKE_SOURCE_DIR}/src/mascii.c"
  "${CMAKE_SOURCE_DIR}/src/mascii.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh.h")
target_include_directories(strchr_benchmark PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(strchr_benchmark benchmark::benchmark)

add_executable(strcspn_benchmark "strcspn_benchmark.cc"
  "${CMAKE_SOURCE_DIR}/src/mascii.c"
  "${CMAKE_SOURCE_DIR}/src/mascii.h"
  "${CMAKE_SOURCE_DIR}/include/fildesh.h")
target_include_directories(strcspn_benchmark PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(strcspn_benchmark benchmark::benchmark)
