load("@bazel_skylib//rules:run_binary.bzl", "run_binary")

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

exports_files([
    "spawn.c",
])

run_binary(
    name = "exec_sh",
    tool = "//src:zec",
    args = [
        "-o",
        "$(location exec.sh)",
        "/",
        "exec \"$@\"",
        "/",
    ],
    outs = ["exec.sh"],
    testonly = True,
    visibility = ["//visibility:public"],
)
