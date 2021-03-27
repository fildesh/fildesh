
add_subdirectory(src)

set (lace_exe "$<TARGET_FILE:lace>")
set (zec_exe "$<TARGET_FILE:zec>")

add_subdirectory(builtin)

## Ensure that the example we distribute actually works.
foreach (f hello test args name familiar cycle convoluted silly)
  add_test (NAME example_${f}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND
    comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/expect/${f}.txt"
    ${lace_exe} -x example/${f}.lace
    )
endforeach ()

add_test (NAME cli_hello
  COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/expect/hello.txt"
  ${lace_exe} -stdout - -- "|< ${zec_exe} / 'hello'"
  )

add_test (NAME example_args2
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/expect/args2.txt"
  ${lace_exe} -x example/args.lace 2 7
  )
