
# For //:fildesh_lib.
exports_files([
    "alloc.c",
    "infile.c",
    "instream.c",
    "kve.c",
    "kve.h",
    "log.c",
    "outfile.c",
    "outstream.c",
    "string.c",
], visibility = [
    "//:__pkg__",
    "//test/fuzz:__pkg__",
    "//test/src:__pkg__",
])

exports_files([
    "execfd.c",
    "godo.c",
    "time2sec.c",
    "void.c",
], visibility = [
    "//test/builtin:__pkg__",
])

cc_library(
    name = "fildesh_builtin_lib",
    srcs = [
        "fildesh.c",
        "execfd.c",
        "godo.c",
        "ssh-all.c",
        "waitdo.c",
        "xpipe.c",
        "//builtin:add.c",
        "//builtin:bestmatch.c",
        "//builtin:cmp.c",
        "//builtin:elastic_pthread.c",
        "//builtin:expect_failure.c",
        "//builtin:seq.c",
        "//builtin:sponge.c",
        "//builtin:time2sec.c",
        "//builtin:transpose.c",
        "//builtin:ujoin.c",
        "//builtin:void.c",
        "//builtin:zec.c",
        "//include:fildesh_builtin.h",
        "//include:fildesh_posix_thread.h",
    ],
    defines = [
        "FILDESH_BUILTIN_LIBRARY",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
)

cc_binary(
    name = "fildesh",
    srcs = ["main_fildesh.c"],
    deps = [":fildesh_builtin_lib"],
    visibility = ["//:__pkg__"],
)

cc_binary(
    name = "chatty",
    srcs = ["chatty.c"],
    deps = ["//:fildesh_lib"],
    linkopts = select({
        "@platforms//os:linux": ["-lrt"],
        "//conditions:default": [],
    }),
    target_compatible_with = [
        "@platforms//os:linux",
    ],
    visibility = ["//test/src:__pkg__"],
)

cc_binary(
    name = "elastic_aio",
    srcs = ["elastic_aio.c"],
    deps = ["//src/cx:cx_lib"],
    linkopts = select({
        "@platforms//os:linux": ["-lrt"],
        "//conditions:default": [],
    }),
    target_compatible_with = [
        "@platforms//os:linux",
    ],
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "elastic_poll",
    srcs = ["elastic_poll.c"],
    deps = ["//src/cx:cx_lib"],
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "execfd",
    srcs = [
        "execfd.c",
        "//include:fildesh_builtin.h",
    ],
    deps = ["//:fildesh_lib"],
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "sshall",
    srcs = ["ssh-all.c"],
    deps = ["//:fildesh_lib"],
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "waitdo",
    srcs = [
        "waitdo.c",
        "//include:fildesh_builtin.h",
    ],
    deps = ["//src/cx:cx_lib"],
    visibility = ["//test/builtin:__pkg__"],
)

