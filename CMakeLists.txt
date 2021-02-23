
cmake_minimum_required (VERSION 2.8.12)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (lace)

enable_testing ()

set (TopPath "${CMAKE_CURRENT_SOURCE_DIR}")
set (TestPath "${TopPath}/test")

## Flags.
if (UNIX)
  foreach (flags_vbl
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
      CMAKE_C_FLAGS_RELWITHDEBINFO)
    string (REGEX REPLACE "(^| )[/-]D *NDEBUG($| )" " "
      "${flags_vbl}" "${${flags_vbl}}")
  endforeach ()

  if (NOT CMAKE_BUILD_TYPE)
    #set (CMAKE_BUILD_TYPE DEBUG)
    set (CMAKE_BUILD_TYPE RelWithDebInfo)
  else ()
  endif ()
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ansi -pedantic")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic")
  set (DEFAULT_COMPILE_FLAGS "-ansi -pedantic")
  #set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
else ()
  set (CMAKE_C_FLAGS "/W4 /MP")
  set (DEFAULT_COMPILE_FLAGS "")
  # Disable warning: 'fopen' unsafe, use fopen_s instead
  add_definitions ("-D_CRT_SECURE_NO_WARNINGS")
  # Other warnings disabled in def.h via #pragmas.

  set (CMAKE_BUILD_TYPE RELEASE)

endif ()

add_subdirectory(src)
add_subdirectory(tool)
add_subdirectory(test)

