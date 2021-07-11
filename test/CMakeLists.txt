
add_subdirectory(compat)
add_subdirectory(tool)
add_subdirectory(src)

set (lace_exe "$<TARGET_FILE:lace>")
set (zec_exe "$<TARGET_FILE:zec>")

add_subdirectory(builtin)
add_subdirectory(fault)

set(plain_example_test_list
  "args"
  "cycle"
  "hello"
  "name"
  )

if (UNIX)
  list(APPEND plain_example_test_list
    "convoluted"
    "familiar"
    "silly"
    "test"
    )
endif()


## Ensure that the examples we distribute actually work.
foreach (f ${plain_example_test_list})
  add_test (NAME example_${f}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND
    comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/expect/${f}.txt"
    ${lace_exe} -x example/${f}.lace
    )
endforeach ()

add_test (NAME example_args2
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/expect/args2.txt"
  ${lace_exe} -x example/args.lace 2 7
  )
