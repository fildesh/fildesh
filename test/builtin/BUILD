load("//tool:comparispawn.bzl", "fildesh_expect_test")
load("//tool:spawn.bzl", "fildesh_test", "spawn_test")

#### add

cc_test(
    name = "add_float_test",
    srcs = ["add_float_test.c", "//builtin:add.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    defines = ["FILDESH_BUILTIN_LIBRARY"],
)

#### bestmatch

cc_test(
    name = "bestmatch_test",
    srcs = ["bestmatch_test.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = ["//src:bestmatch"],
    args = ["$(location //src:bestmatch)"],
)

#### cmp

spawn_test(
    name = "cmp_equal_test",
    args = [
        "$(location //builtin:cmp)",
        "$(location :hello_newline.txt)",
        "$(location :hello_newline.txt)",
        "-o", "-",
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
        "-o", "-",
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

fildesh_expect_test(
    name = "elastic_cat_test",
    srcs = ["elastic_cat.fildesh"],
    expect = ":hello_newline.txt",
)

fildesh_expect_test(
    name = "elastic_tee_test",
    srcs = ["elastic_tee.fildesh"],
    expect = ":hello_newline_x2.txt",
)

cc_test(
    name = "elastic_aio_tee_test",
    srcs = ["elastic_alt_tee_test.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = ["//src:elastic_aio"],
    args = ["$(location //src:elastic_aio)"],
)
cc_test(
    name = "elastic_poll_tee_test",
    srcs = ["elastic_alt_tee_test.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = ["//src:elastic_poll"],
    args = ["$(location //src:elastic_poll)"],
)
cc_test(
    name = "elastic_pthread_tee_test",
    srcs = ["elastic_alt_tee_test.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = ["//builtin:elastic_pthread"],
    args = ["$(location //builtin:elastic_pthread)"],
)

#### execfd

cc_test(
    name = "execfd_test",
    srcs = [
        "execfd_test.c",
        "//include:fildesh_builtin.h",
        "//src:execfd.c",
    ],
    defines = ["UNIT_TESTING"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = ["//builtin:zec"],
    args = ["$(location //builtin:zec)"],
)

fildesh_test(
    name = "execfd_script_test",
    srcs = ["execfd_script_test.fildesh"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
)

#### fildesh

spawn_test(
    name = "fildesh_as_add_test",
    args = [
        "$(location //tool:comparispawn)",
        "$(location summed_integers.txt)",
        "$(location //:fildesh)",
        "-stdin",
        "$(location integers.txt)",
        "-as",
        "add",
    ],
    data = [
        "integers.txt",
        "summed_integers.txt",
        "//:fildesh",
        "//tool:comparispawn",
    ],
)

cc_test(
    name = "fildesh_cli_hello_test",
    srcs = ["fildesh_cli_hello_test.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = ["//:fildesh"],
    args = ["$(location //:fildesh)"],
)

cc_test(
    name = "fildesh_usage_test",
    srcs = ["fildesh_usage_test.c"],
    deps = ["//:fildesh_compat_lib"],
    data = ["//:fildesh"],
    args = ["$(location //:fildesh)"],
)

#### godo

cc_test(
    name = "godo_test",
    srcs = [
        "godo_test.c",
        "//src:godo.c",
    ],
    defines = ["FILDESH_BUILTIN_LIBRARY"],
    deps = [
        "//:fildesh_lib",
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

#### sponge

sh_test(
    name = "sponge_sh_test",
    srcs = ["sponge_test.sh"],
    args = ["$(location //builtin:sponge)"],
    data = ["//builtin:sponge"],
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

#### sshall

cc_binary(
    name = "echocat",
    srcs = [
        "echocat.c",
        "//builtin:zec.c",
    ],
    deps = ["//:fildesh_lib"],
    defines = ["FILDESH_BUILTIN_LIBRARY"],
    testonly = True,
)
fildesh_test(
    name = "echocat_test",
    srcs = ["echocat_test.fildesh"],
    aliases = ["echocat=$(location :echocat)"],
    data = [":echocat"],
)

cc_test(
    name = "sshall_test",
    srcs = ["sshall_test.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = [
        ":echocat",
        "//src:sshall",
    ],
    args = [
        "$(location //src:sshall)",
        "$(location :echocat)"
    ],
)

#### time2sec

cc_test(
    name = "time2sec_test",
    srcs = [
        "time2sec_test.c",
        "//builtin:time2sec.c",
    ],
    defines = ["FILDESH_BUILTIN_LIBRARY"],
    deps = [
        "//:fildesh_lib",
        "//:fildesh_tool_lib",
    ],
)

#### transpose

cc_test(
    name = "transpose_test",
    srcs = [
        "transpose_test.c",
        "//src:transpose.c",
    ],
    defines = ["FILDESH_BUILTIN_LIBRARY"],
    deps = [
        "//:fildesh_tool_lib",
        "//src/cx:cx_lib",
    ],
)

#### ujoin

cc_test(
    name = "ujoin_test",
    srcs = ["ujoin_test.c"],
    deps = ["//:fildesh_lib", "//:fildesh_tool_lib"],
    data = ["//src:ujoin"],
    args = ["$(location //src:ujoin)"],
)

#### void

cc_test(
    name = "void_test",
    args = ["$(location //builtin:void)"],
    srcs = ["void_test.c"],
    deps = [
        "//:fildesh_tool_lib",
        "//builtin:void",
    ],
)

#### waitdo

cc_test(
    name = "waitdo_test",
    args = ["$(location //src:waitdo)", "$(location //tool:shout)"],
    srcs = ["waitdo_test.c"],
    deps = [
        "//:fildesh_lib",
        "//:fildesh_tool_lib",
    ],
    data = [
        "//src:waitdo",
        "//tool:shout",
    ],
)

#### xpipe

fildesh_expect_test(
    name = "xpipe_nested_test",
    srcs = ["xpipe_nested.fildesh"],
    expect = "//test/expect:xpipe_nested.txt",
)

#### zec

fildesh_expect_test(
    name = "zec_cat_test",
    srcs = ["zec_cat.fildesh"],
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
