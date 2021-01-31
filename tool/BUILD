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

genrule(
    name = "exec_sh",
    tools = [
        "//src:zec",
    ],
    cmd = "\"$(location //src:zec)\" -o \"$@\" / 'exec \"$$@\"'",
    outs = ["exec.sh"],
    executable = True,
    testonly = True,
    visibility = ["//visibility:public"],
)
