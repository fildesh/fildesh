load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")


bool_flag(
    name = "enable_full_benchmarking",
    build_setting_default = False,
    visibility = ["//test/benchmark:__subpackages__"],
)

bool_flag(
    name = "enable_full_fuzzing",
    build_setting_default = False,
    visibility = ["//test/fuzz:__subpackages__"],
)
