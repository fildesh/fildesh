
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
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMAND
    comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/${f}.txt"
    "${Fildesh_EXECUTABLE}" -f "example/${f}.fildesh"
  )
endforeach ()

add_test(NAME example_hello_test
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  COMMAND
  comparispawn "${PROJECT_SOURCE_DIR}/test/expect/literal_hello.txt"
  "${Fildesh_EXECUTABLE}" -f "example/hello.fildesh"
)

add_test(NAME example_args2_test
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/args2.txt"
  "${Fildesh_EXECUTABLE}" -f "example/args.fildesh"
  "--offset=2" "--limit=7"
)

add_test(NAME example_args_forkonly_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/args.txt"
  "${Fildesh_EXECUTABLE}" -forkonly -f
  "${CMAKE_CURRENT_SOURCE_DIR}/args_forkonly_test.fildesh"
)

add_test(NAME example_args2_forkonly_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/args2.txt"
  "${Fildesh_EXECUTABLE}" -forkonly -f
  "${CMAKE_CURRENT_SOURCE_DIR}/args_forkonly_test.fildesh"
  "--offset=2" "--limit=7"
)
