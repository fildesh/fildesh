# These are small enough to run many times as cc_test().
exports_files([
    "spawn.c",
], visibility = ["//visibility:public"])

exports_files([
    "argspawn.c",
    "expectish.c",
    "pipem.c",
    "shout.c",
], visibility = [
    "//:__pkg__",
    "//test/tool:__pkg__",
])

cc_binary(
    name = "cembed",
    srcs = [
        "cembed.c",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "comparispawn",
    srcs = [
        "comparispawn.c",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
    copts = [
        "-Isrc",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "cswitch",
    srcs = [
        "cswitch.c",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "expectish",
    srcs = ["expectish.c"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "shout",
    srcs = ["shout.c"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "spawn",
    srcs = ["spawn.c"],
    visibility = ["//visibility:public"],
)
