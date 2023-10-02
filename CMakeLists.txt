
cmake_minimum_required (VERSION 2.8.12)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Fildesh)

include(CTest)


if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(C_ARCH_FLAGS "")
if(CMAKE_BUILD_TYPE MATCHES "RelOnHost.*")
  if (APPLE)
    set(C_ARCH_FLAGS "-march=native")
    set (FILDESH_PREFER_POLL TRUE)
  elseif(UNIX)
    set(C_ARCH_FLAGS "-march=native")
    set (FILDESH_PREFER_AIO TRUE)
    set (FILDESH_BUILTIN_ELASTIC_POLL_ON TRUE)
  else()
    set (FILDESH_PREFER_PTHREAD TRUE)
  endif()
endif()


string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS_RELONHOST "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELONHOST "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELONHOST "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELONHOST "${CMAKE_CXX_FLAGS_RELEASE}")

message(STATUS "CMAKE_C_COMPILER_ID is ${CMAKE_C_COMPILER_ID}")

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
endif()
message(STATUS "CMAKE_C_FLAGS is ${CMAKE_C_FLAGS}")

if (CMAKE_CXX_COMPILER_ID STREQUAL GCC_COMPILER_ID)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic -Werror")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL CLANG_COMPILER_ID)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
  if (NOT APPLE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()
endif()
message(STATUS "CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")


find_package(Threads REQUIRED)

# Conditionally referenced.
set(FILDESH_AIO_LIBS)
list(APPEND FILDESH_AIO_LIBS "rt")
if (FILDESH_PREFER_AIO)
  set (FILDESH_BUILTIN_ELASTIC_AIO_ON TRUE)
endif()
# Conditionally referenced.
set(FILDESH_POLL_LIBS)
if (APPLE)
  list(APPEND FILDESH_POLL_LIBS "c")
endif()
if (FILDESH_PREFER_POLL)
  set (FILDESH_BUILTIN_ELASTIC_POLL_ON TRUE)
endif()


set(Fildesh_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories(
  ${Fildesh_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}"
)
include(GNUInstallDirs)
add_subdirectory(compat)
add_subdirectory(src)
set(Fildesh_LIBRARIES "$<TARGET_NAME:fildesh_lib>")
set(FildeshSxproto_LIBRARIES "$<TARGET_NAME:sxproto_lib>" "$<TARGET_NAME:fildesh_lib>")


if (NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(Fildesh_INCLUDE_DIRS "${Fildesh_INCLUDE_DIRS}" PARENT_SCOPE)
  set(Fildesh_LIBRARIES ${Fildesh_LIBRARIES} PARENT_SCOPE)
  set(FildeshSxproto_LIBRARIES ${FildeshSxproto_LIBRARIES} PARENT_SCOPE)
  # Try to keep dependent project namespace clean.
  # No need for tests or anything else.
  return()
endif()

add_subdirectory(tool)
if (BUILD_TESTING)
  add_subdirectory(test)
endif()
add_subdirectory(pkg)

