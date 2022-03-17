
exports_files([
    "add.c",
    "cmp.c",
    "elastic_pthread.c",
    "expect_failure.c",
    "seq.c",
    "sponge.c",
    "time2sec.c",
    "void.c",
    "zec.c",
], visibility = [
    "//test/builtin:__pkg__",
    "//src:__pkg__",
])

cc_binary(
    name = "add",
    srcs = ["add.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "cmp",
    srcs = ["cmp.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "elastic_pthread",
    srcs = [
        "elastic_pthread.c",
        "//include:fildesh_posix_thread.h",
    ],
    deps = ["//:fildesh_lib"],
    linkopts = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "expect_failure",
    srcs = ["expect_failure.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "seq",
    srcs = ["seq.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "sponge",
    srcs = ["sponge.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "time2sec",
    srcs = ["time2sec.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "void",
    srcs = ["void.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "zec",
    srcs = ["zec.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//visibility:public"],
)

