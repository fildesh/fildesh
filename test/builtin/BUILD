load("//tool:comparispawn.bzl", "lace_expect_test")
load("//tool:spawn.bzl", "spawn_test")

#### add

spawn_test(
    name = "lace_as_add_test",
    args = [
        "$(location //tool:comparispawn)",
        "$(location summed_integers.txt)",
        "$(location //:lace)",
        "-stdinfile",
        "$(location integers.txt)",
        "-as",
        "add",
    ],
    data = [
        "integers.txt",
        "summed_integers.txt",
        "//:lace",
        "//tool:comparispawn",
    ],
)

#### cmp

spawn_test(
    name = "cmp_equal_test",
    args = [
        "$(location //builtin:cmp)",
        "$(location :hello_newline.txt)",
        "$(location :hello_newline.txt)",
    ],
    data = [
        ":hello_newline.txt",
        "//builtin:cmp",
    ],
)
spawn_test(
    name = "cmp_diff_test",
    args = [
        "!",
        "$(location //builtin:cmp)",
        "$(location :hello_newline.txt)",
        "$(location :hello_newline_x2.txt)",
    ],
    data = [
        ":hello_newline.txt",
        ":hello_newline_x2.txt",
        "//builtin:cmp",
    ],
)

#### elastic

lace_expect_test(
    name = "elastic_cat_test",
    srcs = ["elastic_cat.lace"],
    expect = ":hello_newline.txt",
)

lace_expect_test(
    name = "elastic_tee_test",
    srcs = ["elastic_tee.lace"],
    expect = ":hello_newline_x2.txt",
)

#### godo

cc_test(
    name = "godo_test",
    srcs = [
        "godo_test.c",
        "//src:godo.c",
    ],
    defines = ["LACE_BUILTIN_LIBRARY"],
    deps = [
        "//:lace_lib",
    ],
    data = [
        "//tool:expectish",
        "//test/expect:hello.txt",
    ],
    args = [
        "$(location //tool:expectish)",
        "$(location //test/expect:hello.txt)",
    ],
)

#### time2sec

cc_test(
    name = "time2sec_test",
    srcs = [
        "time2sec_test.c",
        "//builtin:time2sec.c",
    ],
    defines = ["LACE_BUILTIN_LIBRARY"],
    deps = [
        "//:lace_lib",
        "//:lace_tool_lib",
    ],
)

#### transpose

cc_test(
    name = "transpose_test",
    srcs = [
        "transpose_test.c",
        "//src:transpose.c",
    ],
    defines = ["LACE_BUILTIN_LIBRARY"],
    deps = [
        "//:lace_tool_lib",
        "//src/cx:cx_lib",
    ],
)

#### void

cc_test(
    name = "void_test",
    args = ["$(location //builtin:void)"],
    srcs = ["void_test.c"],
    deps = [
        "//:lace_tool_lib",
        "//builtin:void",
    ],
)

#### xpipe

lace_expect_test(
    name = "xpipe_nested_test",
    srcs = ["xpipe_nested.lace"],
    expect = "//test/expect:xpipe_nested.txt",
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

#### zec

lace_expect_test(
    name = "zec_cat_test",
    srcs = ["zec_cat.lace"],
    expect = ":hello_newline.txt",
)

spawn_test(
    name = "zec_nop_test",
    data = ["//builtin:zec"],
    args = ["$(location //builtin:zec)", "/", "/"],
)

spawn_test(
    name = "zec_usage_test",
    data = ["//builtin:zec"],
    args = ["$(location //builtin:zec)", "-h"],
)
