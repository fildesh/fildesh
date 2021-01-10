
cc_binary(
    name = "lace",
    srcs = [
        "lace.c",
        "utilace.h",
        "add.c",
        "best-match.c",
        "elastic.c",
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
    linkopts = [
        "-lrt",
    ],
)


cc_binary(
    name = "chatty",
    srcs = [
        "chatty.c",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
    linkopts = [
        "-lrt",
    ],
)

