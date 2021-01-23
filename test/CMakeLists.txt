
set (lace_exe "$<TARGET_FILE:lace>")
set (zec_exe "$<TARGET_FILE:zec>")

## Ensure that the example we distribute actually works.
foreach (f hello test args name familiar cycle convoluted silly)
  add_test (NAME example_${f}
    WORKING_DIRECTORY ${TopPath}
    COMMAND
    comparispawn ${TestPath}/expect/${f}.txt
    ${lace_exe} -x example/${f}.lace
    )
endforeach ()

## Tests in this directory.
foreach (f xpipe_nested)
  add_test (NAME ${f}
    WORKING_DIRECTORY ${TopPath}
    COMMAND
    comparispawn ${TestPath}/expect/${f}.txt
    ${lace_exe} -x ${TestPath}/${f}.lace
    )
endforeach ()

add_test (NAME cli_hello
  COMMAND
  comparispawn ${TestPath}/expect/hello.txt
  ${lace_exe} -stdout - -- "|< ${zec_exe} / 'hello'"
  )

add_test (NAME example_args2
  WORKING_DIRECTORY ${TopPath}
  COMMAND
  comparispawn ${TestPath}/expect/args2.txt
  ${lace_exe} -x example/args.lace 2 7
  )
