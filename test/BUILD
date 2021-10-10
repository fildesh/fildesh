load("//tool:comparispawn.bzl", "fildesh_expect_test")
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


fildesh_expect_test(
    name = "args_test",
    srcs = ["//example:args.fildesh"],
    expect = "//test/expect:args.txt",
)

fildesh_expect_test(
    name = "args2_test",
    srcs = ["//example:args.fildesh"],
    expect = "//test/expect:args2.txt",
    args = [
        "2",
        "7",
    ],
)

fildesh_expect_test(
    name = "convoluted_test",
    srcs = ["//example:convoluted.fildesh"],
    expect = "//test/expect:convoluted.txt",
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

fildesh_expect_test(
    name = "cycle_test",
    srcs = ["//example:cycle.fildesh"],
    expect = "//test/expect:cycle.txt",
)

fildesh_expect_test(
    name = "familiar_test",
    srcs = ["//example:familiar.fildesh"],
    expect = "//test/expect:familiar.txt",
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

fildesh_expect_test(
    name = "hello_test",
    srcs = ["//example:hello.fildesh"],
    expect = "//test/expect:hello.txt",
)

fildesh_expect_test(
    name = "name_test",
    srcs = [
        "//example:name.fildesh",
        "//example:includetest.fildesh",
    ],
    expect = "//test/expect:name.txt",
)

fildesh_expect_test(
    name = "silly_test",
    srcs = ["//example:silly.fildesh"],
    expect = "//test/expect:silly.txt",
    data = [
        "//example:args.fildesh",
        "//example:cycle.fildesh",
        "//example:hello.fildesh",
    ],
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

fildesh_expect_test(
    name = "test_test",
    srcs = ["//example:test.fildesh"],
    expect = "//test/expect:test.txt",
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)
