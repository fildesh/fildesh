load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")


def fildesh_dependencies():
  maybe(
      http_archive,
      name = "platforms",
      sha256 = "5308fc1d8865406a49427ba24a9ab53087f17f5266a7aabbfc28823f3916e1ca",
      urls = [
          "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
          "https://github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
      ],
  )

  maybe(
      http_archive,
      name = "bazel_skylib",
      sha256 = "74d544d96f4a5bb630d465ca8bbcfe231e3594e5aae57e1edbf17a6eb3ca2506",
      urls = [
          "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
          "https://github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
      ],
  )



def fildesh_dev_dependencies():
  fildesh_dependencies()

  maybe(
      http_archive,
      name = "google_benchmark",
      sha256 = "3aff99169fa8bdee356eaa1f691e835a6e57b1efeadb8a0f9f228531158246ac",
      strip_prefix = "benchmark-1.7.0",
      urls = ["https://github.com/google/benchmark/archive/refs/tags/v1.7.0.tar.gz"],
  )
