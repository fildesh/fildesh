
cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (protocon)

enable_testing ()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set (TopPath "${CMAKE_CURRENT_SOURCE_DIR}")
set (TestPath "${TopPath}/test")

add_subdirectory(src)

