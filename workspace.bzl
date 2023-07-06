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


def fildesh_dev_dependencies():
  fildesh_dependencies()

  maybe(
      http_archive,
      name = "google_benchmark",
      sha256 = "2aab2980d0376137f969d92848fbb68216abb07633034534fc8c65cc4e7a0e93",
      strip_prefix = "benchmark-1.8.2",
      urls = ["https://github.com/google/benchmark/archive/v1.8.2.tar.gz"],
  )
