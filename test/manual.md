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

### Leak Check
```shell
bazel build //src:fildesh
./bazel-bin/src/fildesh s/leak_check.fildesh
# Don't worry if the `xargs ... grep ...` command fails.
# Grep exits with a nonzero status because some
# (hopefully most) testlogs do not have messages about leaks.
```

### Lint
```shell
# Casting function pointers is usually unsafe (https://stackoverflow.com/a/559671/5039395).
grep -E -e '\([^()]*\(\*\) *\([^()]*\)' -R builtin compat src test tool
```
