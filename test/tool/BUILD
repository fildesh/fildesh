
cc_binary(
    name = "success",
    srcs = ["success.c"],
    testonly = True,
)
cc_binary(
    name = "failure",
    srcs = ["failure.c"],
    testonly = True,
)

cc_test(
    name = "success_test",
    srcs = ["success.c"],
)
cc_test(
    name = "spawn_success_test",
    srcs = ["//tool:spawn.c"],
    args = ["$(location :success)"],
    data = [":success"],
)
cc_test(
    name = "spawn_not_failure_test",
    srcs = ["//tool:spawn.c"],
    args = ["!", "$(location :failure)"],
    data = [":failure"],
)
cc_test(
    name = "spawn_not_spawn_failure_test",
    srcs = ["//tool:spawn.c"],
    args = ["!", "$(location //tool:spawn)", "$(location :failure)"],
    data = [":failure", "//tool:spawn"],
)
cc_test(
    name = "spawn_not_spawn_not_success_test",
    srcs = ["//tool:spawn.c"],
    args = ["!", "$(location //tool:spawn)", "!", "$(location :success)"],
    data = ["//tool:spawn", ":success"],
)
cc_test(
    name = "spawn_not_spawn_invalid_test",
    srcs = ["//tool:spawn.c"],
    args = ["!", "$(location //tool:spawn)", "."],
    data = ["//tool:spawn"],
)
cc_test(
    name = "spawn_not_spawn_not_invalid_test",
    srcs = ["//tool:spawn.c"],
    args = ["!", "$(location //tool:spawn)", "!", "."],
    data = ["//tool:spawn"],
)
cc_test(
    name = "spawn_not_spawn_absent_test",
    srcs = ["//tool:spawn.c"],
    args = ["!", "$(location //tool:spawn)", "$(location //tool:spawn).doesnotexist"],
    data = ["//tool:spawn"],
)
cc_test(
    name = "spawn_not_spawn_not_absent_test",
    srcs = ["//tool:spawn.c"],
    args = ["!", "$(location //tool:spawn)", "!", "$(location //tool:spawn).doesnotexist"],
    data = ["//tool:spawn"],
)
