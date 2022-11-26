
cmake_minimum_required (VERSION 2.8.12)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(fildesh)

enable_testing()


if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelOnHost)
endif()

set(FILDESH_TESTING_RELEASE FALSE)
set(FILDESH_RELEASING FALSE)
if (CMAKE_BUILD_TYPE MATCHES "Release.*")
  set(FILDESH_RELEASING TRUE)
elseif(CMAKE_BUILD_TYPE MATCHES "RelOnHost.*")
  set(FILDESH_TESTING_RELEASE TRUE)
endif()

set(C_ARCH_FLAGS "")
if (FILDESH_TESTING_RELEASE)
  # Not referenced on Windows.
  set(C_ARCH_FLAGS "-march=native")
endif()


string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS_RELONHOST "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELONHOST "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELONHOST "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELONHOST "${CMAKE_CXX_FLAGS_RELEASE}")

message("INFO: CMAKE_C_COMPILER_ID is ${CMAKE_C_COMPILER_ID}")

set(GCC_COMPILER_ID "GNU")
set(CLANG_COMPILER_ID "Clang")
set(TCC_COMPILER_ID "TinyCC")
set(MSVC_COMPILER_ID "MSVC")

if (CMAKE_C_COMPILER_ID STREQUAL GCC_COMPILER_ID)
  # GCC.
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_ARCH_FLAGS} -Wall -Wextra -ansi -pedantic -Werror")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_ARCH_FLAGS}")
elseif (CMAKE_C_COMPILER_ID STREQUAL CLANG_COMPILER_ID)
  # Clang.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_ARCH_FLAGS} -Wall -Wextra -pedantic")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_ARCH_FLAGS}")
  if (NOT APPLE)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
  endif()
elseif (CMAKE_C_COMPILER_ID STREQUAL TCC_COMPILER_ID)
  # TinyCC:
  #  cmake -D CMAKE_C_COMPILER=tcc ..
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_ARCH_FLAGS} -Wall -Wunsupported -Wwrite-strings -Werror")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_ARCH_FLAGS}")
elseif (CMAKE_C_COMPILER_ID STREQUAL MSVC_COMPILER_ID)
  # Windows.
  set (CMAKE_C_FLAGS "/favor:ATOM /W4 /MP")
  set (CMAKE_CXX_FLAGS "/favor:ATOM")
  # Disable warning: 'fopen' unsafe, use fopen_s instead
  add_definitions ("-D_CRT_SECURE_NO_WARNINGS")
  # See src/cx/def.h for how to disable other warnings via #pragmas.
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL GCC_COMPILER_ID)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic -Werror")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL CLANG_COMPILER_ID)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
  if (NOT APPLE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()
endif()

find_package(Threads REQUIRED)

include_directories(include)
include_directories(include/fildesh)
include(GNUInstallDirs)
add_subdirectory(compat)
add_subdirectory(src)
add_subdirectory(tool)
add_subdirectory(test)
add_subdirectory(pkg)

