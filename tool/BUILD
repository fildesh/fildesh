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
    visibility = ["//visibility:public"],
)

cc_test(
    name = "spawn_not_not_test",
    srcs = ["spawn.c"],
    args = ["!", "!"],
)
cc_test(
    name = "spawn_spawn_not_not_test",
    srcs = ["spawn.c"],
    args = ["$(location :spawn)", "!", "!"],
    data = [":spawn"],
)
cc_test(
    name = "spawn_not_spawn_not_test",
    srcs = ["spawn.c"],
    args = ["!", "$(location :spawn)", "!"],
    data = [":spawn"],
)
cc_test(
    name = "spawn_not_spawn_nothing_test",
    srcs = ["spawn.c"],
    args = ["!", "$(location :spawn)"],
    data = [":spawn"],
)
