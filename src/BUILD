
# For //:lace_lib.
exports_files([
    "infile.c",
    "instream.c",
    "outfile.c",
    "outstream.c",
    "string.c",
])

cc_binary(
    name = "lace",
    srcs = [
        "lace.c",
        "utilace.h",
        "add.c",
        "best-match.c",
        #"elastic_poll.c",
        "elastic_pthread.c",
        "execfd.c",
        "godo.c",
        "ssh-all.c",
        "time2sec.c",
        "transpose.c",
        "ujoin.c",
        "void.c",
        "waitdo.c",
        "xpipe.c",
        "zec.c",
    ],
    defines = [
        "MAIN_LACE_EXECUTABLE",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
    linkopts = ["-lpthread"],
    visibility = ["//:__pkg__"],
)


cc_binary(
    name = "add",
    srcs = [
        "add.c",
    ],
    deps = [
        "//:lace_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "elastic_pthread",
    srcs = [
        "elastic_pthread.c",
        "utilace.h",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "zec",
    srcs = [
        "zec.c",
    ],
    deps = [
        "//:lace_lib",
    ],
    visibility = ["//visibility:public"],
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
