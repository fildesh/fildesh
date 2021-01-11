
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
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "zec",
    srcs = [
        "zec.c",
        "utilace.h",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
    visibility = ["//visibility:public"],
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

cc_binary(
    name = "comparispawn",
    srcs = [
        "comparispawn.c",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
    visibility = ["//visibility:public"],
)

genrule(
    name = "comparispawn_lace_sh",
    srcs = [
        ":comparispawn",
        ":lace",
    ],
    tools = [
        ":zec",
    ],
    cmd = "\"$(location zec)\" -o $@ / 'expect=\"$$1\"; shift; exec \"$(location :comparispawn)\" \"$$expect\" \"$(location :lace)\" \"$$@\"'",
    outs = ["comparispawn_lace.sh"],
    executable = True,
    testonly = True,
    visibility = ["//visibility:public"],
)

genrule(
    name = "exec_sh",
    tools = [
        ":zec",
    ],
    cmd = "\"$(location zec)\" -o \"$@\" / 'exec \"$$@\"'",
    outs = ["exec.sh"],
    executable = True,
    testonly = True,
    visibility = ["//visibility:public"],
)
