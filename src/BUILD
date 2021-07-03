
# For //:lace_lib.
exports_files([
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
    "godo.c",
    "time2sec.c",
    "transpose.c",
    "void.c",
], visibility = [
    "//test/builtin:__pkg__",
])

cc_binary(
    name = "lace",
    srcs = [
        "lace.c",
        "utilace.h",
        "best-match.c",
        #"elastic_poll.c",
        "execfd.c",
        "godo.c",
        "ssh-all.c",
        "transpose.c",
        "ujoin.c",
        "waitdo.c",
        "xpipe.c",
        "//builtin:add.c",
        "//builtin:cmp.c",
        "//builtin:elastic_pthread.c",
        "//builtin:seq.c",
        "//builtin:time2sec.c",
        "//builtin:void.c",
        "//builtin:zec.c",
        "//include:lace_builtin.h",
        "//include:lace_posix_thread.h",
    ],
    defines = [
        "LACE_BUILTIN_LIBRARY",
    ] + select({
        "@platforms//os:windows": ["LACE_BUILTIN_FORBID_XPIPE"],
        "//conditions:default": [],
    }),
    deps = [
        "//src/cx:cx_lib",
    ],
    linkopts = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    visibility = ["//:__pkg__"],
)


#cc_binary(
#    name = "chatty",
#    srcs = [
#        "chatty.c",
#    ],
#    deps = [
#        "//src/cx:cx_lib",
#    ],
#    linkopts = select({
#        ":linux_os": ["-lrt"],
#        ":mac_osx": ["-lc"],
#    }),
#    target_compatible_with = [
#        "@bazel_tools//platforms:linux",
#    ],
#)
