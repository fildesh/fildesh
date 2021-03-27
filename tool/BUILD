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
cc_binary(
    name = "spawn",
    srcs = ["spawn.c"],
)
cc_binary(
    name = "success",
    srcs = ["success.c"],
)
cc_binary(
    name = "failure",
    srcs = ["failure.c"],
)
cc_test(
    name = "success_test",
    srcs = ["spawn.c"],
    args = ["$(location :success)"],
    data = [":success"],
)
cc_test(
    name = "failure_test",
    srcs = ["spawn.c"],
    args = ["!", "$(location :failure)"],
    data = [":failure"],
)
