[![Bazel](https://github.com/fildesh/fildesh/actions/workflows/bazel.yml/badge.svg)](https://github.com/fildesh/fildesh/actions/workflows/bazel.yml)
[![CMake](https://github.com/fildesh/fildesh/actions/workflows/cmake.yml/badge.svg)](https://github.com/fildesh/fildesh/actions/workflows/cmake.yml)
\
[![Coverage Status](https://coveralls.io/repos/github/fildesh/fildesh/badge.svg?branch=trunk)](https://coveralls.io/github/fildesh/fildesh?branch=trunk)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/6377/badge)](https://bestpractices.coreinfrastructure.org/projects/6377)


# Fildesh: File Descriptor Shell Scripting Language

Fildesh is a domain-specific language meant to simplify complex pipelines between programs.
It allows multiple inputs and outputs, undirected cycles ([example/familiar.fildesh](example/familiar.fildesh)), and even directed cycles ([example/cycle.fildesh](example/cycle.fildesh)).
Though when the dataflow graph has undirected cycles, you should use the `elastic` utility to avoid deadlocks.

## Motivating Example

Diff tests are a simple way of testing program behavior, but they usually involve multiple files.
A Fildesh script can instead create those multiple files as pipes, letting the whole test be in one place.

```shell
# Pretend that these first 3 lines are just one program that we're testing,
# expecting it to sum the integers from 1 to 10.
|< seq 1 10
|- tr "\n" " "
|- add
# Redirect output to a stream named `result`.
|> zec -o $(OF result)

# Make a stream named `expect` that contains the expected result: "55\n".
zec -o $(OF expect) / "55\n" /

# Compare the two streams. The script fails if the streams differ.
|< cmp $(XF expect) $(XF result)
|> stdout
```

## How to Use

If you have Bazel, try:
```shell
bazel run //:fildesh -- $PWD/example/hello.fildesh
```

If you have CMake, try:
```shell
make && ./bld/src/fildesh example/hello.fildesh
```
Or if you must use CMake directly:
```shell
mkdir -p bld; cd bld
cmake .. && cmake --build .
cd ..
./bld/src/fildesh example/hello.fildesh
```

