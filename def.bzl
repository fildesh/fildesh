# TODO(#146): Remove this file in v0.2.0.
load("//tool/bazel:fildesh.bzl", _fildesh_test = "fildesh_test")
load("//tool/bazel:spawn.bzl", _spawn_test = "spawn_test")

fildesh_test = _fildesh_test
spawn_test = _spawn_test

