# Manual Testing

Automate everything, but remember how it actually works.

## Quick Reference

### Coverage
```shell
bazel coverage --instrument_test_targets --instrumentation_filter="^//..." --combined_report=lcov //...
genhtml bazel-out/_coverage/_coverage_report.dat --output-directory coverage
chromium coverage/index.html
```

### Fuzz Test
```shell
bazel run --config=asan-libfuzzer //test/fuzz:grow_mpop_fuzz_test_run
```

### Valgrind Debugging
```shell
bazel test -c dbg --test_output=all --cache_test_results=no --run_under='valgrind --trace-children=yes' //test/src:kve_test
```

