# Manual Testing

Automate everything, but remember how it actually works.

## Quick Reference

### Benchmark
```shell
bazel test --config=benchmark //test/benchmark/...
```

### Coverage
```shell
bazel coverage --instrument_test_targets --instrumentation_filter="^//..." --combined_report=lcov //...
genhtml bazel-out/_coverage/_coverage_report.dat --output-directory coverage
chromium coverage/index.html
```

### Fuzz Test
```shell
bazel run --config=asan-libfuzzer //test/fuzz:grow_mpop_fuzz_test_full
# Full fuzz tests never terminate.
# Get better info if there's an error with: -c dbg
```

### Build System Parity
```shell
make
make test | grep 'Test *#' | wc -l
bazel test -- //... -//test/fuzz/... | grep '^//' | wc -l
# Bazel and CMake should run the same number of tests on Linux.
# If CMake is short by 5, try rerunning Bazel tests with `-//test/benchmark/...`
# to exclude benchmarks. CMake only runs those if Google Benchmark is installed.
```

### Valgrind Debugging
```shell
bazel test -c dbg --test_output=all --cache_test_results=no --run_under='valgrind --trace-children=yes' //test/src:kve_test
```

### Leak Check
```shell
bazel build //src:fildesh
./bazel-bin/src/fildesh test/manual/leak_check.fildesh
# Don't worry if the `xargs ... grep ...` command fails.
# Grep exits with a nonzero status because some
# (hopefully most) testlogs do not have messages about leaks.
```

### Lint
```shell
# Casting function pointers is usually unsafe (https://stackoverflow.com/a/559671/5039395).
grep -E -e '\([^()]*\(\*\) *\([^()]*\)' -R builtin compat src test tool
```

### SIMD Instructions
```shell
objdump -M intel --disassemble=find_FildeshMascii bld/test/benchmark/strcspn_benchmark | cut -f3
```

### TinyCC Compilation
```shell
mkdir -p bld
cd bld
cmake -D CMAKE_C_COMPILER=tcc ..
make
```
