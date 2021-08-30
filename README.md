[![Bazel](https://github.com/grencez/lace/actions/workflows/bazel.yml/badge.svg)](https://github.com/grencez/lace/actions/workflows/bazel.yml)
[![CMake](https://github.com/grencez/lace/actions/workflows/cmake.yml/badge.svg)](https://github.com/grencez/lace/actions/workflows/cmake.yml)
\
[![Coverage Status](https://coveralls.io/repos/github/grencez/lace/badge.svg?branch=trunk)](https://coveralls.io/github/grencez/lace?branch=trunk)

# The Lace Programming Language

This is a domain-specific language meant to simplify complex pipelines between programs.
It allows multiple inputs and outputs, undirected cycles ([example/familiar.lace](example/familiar.lace)), and even directed cycles ([example/cycle.lace](example/cycle.lace)).
Though when the dataflow graph has undirected cycles, you should use the `elastic` utility to avoid deadlocks.

## How to Use

```
make
./bin/lace < example/hello.lace
```

Hopefully that worked, the only real dependencies are `git` (for pulling submodules), `cmake`, a C compiler, and the POSIX librt for the `elastic` tool.

## Motivating Example

Let's walk through a fairly useful script!
Open [example/whophys.lace](example/whophys.lace) in a new window and follow along.

The `whophys` script is made for instructors to easily (or magically) learn the names of students sitting in a computer lab.
It does this by taking the layout of the computer lab as a variable `lab` and replacing each machine name with the username of whoever is logged in.

The `lab` is defined as a Here document, much like in shell scripting.
The entire text between two occurrences of `$(H lab)` (on separate lines) is then stored in that variable.
All variables are referenced this way, where the prefix (`H` in this case) indicates both type and intent.

Next, a `zec` command prints the lab text to a stream `a`.
The stream is defined by `$(O a)`, where the `O` indicates output.
Notice from the next comment that we could alternatively use `stdin` to read a lab description from standard input.

We will need to reference the lab description twice, so use `tee` to make a copy of the `a` stream to a stream `b`.
This line uses `$(XO a)` to say that `a` is used for both input (`X`) and output (`O`).
The stream `b` is introduced as `$(OF b)`, which means it is an output (`O`) file (`F`) passed as an argument to `tee`.
It is a stream though, just passed to `tee` as `/dev/fd/3` (or some other number).

When using `tee` to save a stream for later, we must use `elastic` on the slow stream to ensure no deadlocks occur.
The `elastic` tool asynchronously reads and writes whenever possible, thereby providing a dynamic buffer.

The next 3 lines just `grep` out the hostnames in the lab description.
These are piped to `xargs` which runs `ssh` to execute `clientcmd` on every host.
At this point, each line has a hostname and a username which are then transformed by `awk` into a `sed` script to do a bunch of search and replace operations.
The `b` stream holding a copy of the lab description is finally used here when `sed` replaces hostnames with usernames.
This output is then piped to `stdout` to display.

