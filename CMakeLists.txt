
cmake_minimum_required (VERSION 2.8.12)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (lace)

enable_testing ()

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif()

string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

## Flags.
if (UNIX)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ansi -pedantic")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic")
  if (NOT APPLE)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
  endif()
else()
  set (CMAKE_C_FLAGS "/W4 /MP")
  # Disable warning: 'fopen' unsafe, use fopen_s instead
  add_definitions ("-D_CRT_SECURE_NO_WARNINGS")
  # Other warnings disabled in def.h via #pragmas.
endif()

find_package(Threads REQUIRED)

include_directories(include)
add_subdirectory(compat)
add_subdirectory(src)
add_subdirectory(builtin)
add_subdirectory(tool)
add_subdirectory(test)

