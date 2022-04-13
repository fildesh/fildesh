
set(plain_example_test_list
  "args"
  "cycle"
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
  add_test(NAME example_${f}_test
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND
    comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/${f}.txt"
    ${fildesh_exe} -f "example/${f}.fildesh"
    )
endforeach ()

add_test(NAME example_hello_test
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMAND
  comparispawn "${CMAKE_SOURCE_DIR}/test/expect/literal_hello.txt"
  ${fildesh_exe} -f "example/hello.fildesh"
  )

add_test(NAME example_args2_test
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/args2.txt"
  ${fildesh_exe} -f "example/args.fildesh" 2 7
  )
