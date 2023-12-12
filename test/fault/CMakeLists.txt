
add_test(NAME fault_barrier_bail_test COMMAND ${Fildesh_EXECUTABLE}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/barrier_bail_test.fildesh"
)

add_test(NAME fault_executable_bytes_of_test COMMAND ${FildeshTool_spawn_EXECUTABLE}
  ! "${Fildesh_EXECUTABLE}"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/executable_bytes_of_test.fildesh"
  "--input_exe=${FildeshTool_shout_EXECUTABLE}"
)

add_test(NAME fault_false_test COMMAND ${FildeshTool_spawn_EXECUTABLE}
  ! "${Fildesh_EXECUTABLE}"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/false_test.fildesh"
)

add_test(NAME fault_include_missing_test COMMAND ${FildeshTool_spawn_EXECUTABLE}
  ! "${Fildesh_EXECUTABLE}"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/include_missing_test.fildesh"
)

add_test(NAME fault_pump_executable_bytes_oxf_test COMMAND ${FildeshTool_spawn_EXECUTABLE}
  ! "${Fildesh_EXECUTABLE}"
  -f "${CMAKE_CURRENT_SOURCE_DIR}/pump_executable_bytes_oxf_test.fildesh"
  "--input_exe=${FildeshTool_shout_EXECUTABLE}"
)

