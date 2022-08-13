load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")


def fildesh_dependencies(bzlmod_on=False):
  if bzlmod_on:
    return

  maybe(
      http_archive,
      name = "platforms",
      sha256 = "079945598e4b6cc075846f7fd6a9d0857c33a7afc0de868c2ccb96405225135d",
      urls = [
          "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.4/platforms-0.0.4.tar.gz",
          "https://github.com/bazelbuild/platforms/releases/download/0.0.4/platforms-0.0.4.tar.gz",
      ],
  )


def fildesh_dev_dependencies(bzlmod_on=False):
  fildesh_dependencies(bzlmod_on=bzlmod_on)

  maybe(
      http_archive,
      name = "rules_fuzzing",
      sha256 = "23bb074064c6f488d12044934ab1b0631e8e6898d5cf2f6bde087adb01111573",
      strip_prefix = "rules_fuzzing-0.3.1",
      urls = ["https://github.com/bazelbuild/rules_fuzzing/archive/v0.3.1.zip"],
  )

  if bzlmod_on:
    return

  maybe(
      http_archive,
      name = "google_benchmark",
      sha256 = "3aff99169fa8bdee356eaa1f691e835a6e57b1efeadb8a0f9f228531158246ac",
      strip_prefix = "benchmark-1.7.0",
      urls = ["https://github.com/google/benchmark/archive/refs/tags/v1.7.0.tar.gz"],
  )
