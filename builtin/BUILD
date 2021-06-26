
exports_files([
    "add.c",
    "cmp.c",
    "elastic_pthread.c",
    "time2sec.c",
    "void.c",
    "zec.c",
], visibility = ["//visibility:public"])

cc_binary(
    name = "add",
    srcs = ["add.c"],
    deps = ["//:lace_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "cmp",
    srcs = ["cmp.c"],
    deps = ["//:lace_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "elastic_pthread",
    srcs = [
        "elastic_pthread.c",
        "//include:lace_posix_thread.h",
    ],
    deps = ["//:lace_lib"],
    linkopts = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "void",
    srcs = ["void.c"],
    deps = ["//:lace_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "zec",
    srcs = ["zec.c"],
    deps = ["//:lace_lib"],
    visibility = ["//visibility:public"],
)

