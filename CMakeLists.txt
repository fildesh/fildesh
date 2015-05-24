
cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

include(ExternalProject)

set (TopPath ${CMAKE_CURRENT_SOURCE_DIR})
set (DepPath ${TopPath}/dep)

ExternalProject_Add(cx-pp
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/cx-pp
  SOURCE_DIR ${DepPath}/cx-pp
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND echo "No configure step."
  BUILD_COMMAND echo "No build step."
  INSTALL_COMMAND echo "No install step."
  )

ExternalProject_Add(cx
  DEPENDS cx-pp
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/cx
  SOURCE_DIR ${DepPath}/cx
  INSTALL_COMMAND echo "No install step."
  )

ExternalProject_Add(exc
  DEPENDS cx
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/src
  SOURCE_DIR ${TopPath}/src
  BINARY_DIR ${TopPath}/bld
  CONFIGURE_COMMAND ${CMAKE_COMMAND}
  ../src
  INSTALL_COMMAND echo "No install step."
  )

