load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")


def fildesh_dependencies():
  maybe(
      http_archive,
      name = "platforms",
      sha256 = "3a561c99e7bdbe9173aa653fd579fe849f1d8d67395780ab4770b1f381431d51",
      urls = [
          "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.7/platforms-0.0.7.tar.gz",
          "https://github.com/bazelbuild/platforms/releases/download/0.0.7/platforms-0.0.7.tar.gz",
      ],
  )
  maybe(
      http_archive,
      name = "rules_license",
      sha256 = "4531deccb913639c30e5c7512a054d5d875698daeb75d8cf90f284375fe7c360",
      urls = [
          "https://mirror.bazel.build/github.com/bazelbuild/rules_license/releases/download/0.0.7/rules_license-0.0.7.tar.gz",
          "https://github.com/bazelbuild/rules_license/releases/download/0.0.7/rules_license-0.0.7.tar.gz",
      ],
  )


def fildesh_dev_dependencies():
  fildesh_dependencies()

  maybe(
      http_archive,
      name = "google_benchmark",
      sha256 = "6bc180a57d23d4d9515519f92b0c83d61b05b5bab188961f36ac7b06b0d9e9ce",
      strip_prefix = "benchmark-1.8.3",
      urls = ["https://github.com/google/benchmark/archive/v1.8.3.tar.gz"],
  )
