load("//tool:spawn.bzl", "spawn_test")

#### comparispawn

spawn_test(
    name = "comparispawn_diff_test",
    args = [
        "!",
        "$(location //tool:comparispawn)",
        "$(location //test/expect:hello.txt)",
        "$(location //tool:shout)", "-", "hel", "lo",
    ],
    data = [
        "//tool:comparispawn",
        "//tool:shout",
        "//test/expect:hello.txt",
    ],
)
spawn_test(
    name = "comparispawn_shout_hello_test",
    args = [
        "$(location //tool:comparispawn)",
        "$(location //test/expect:hello.txt)",
        "$(location //tool:shout)", "-", "hello",
    ],
    data = [
        "//tool:comparispawn",
        "//tool:shout",
        "//test/expect:hello.txt",
    ],
)

#### expectish

cc_test(
    name = "expectish_hello_test",
    srcs = ["//tool:expectish.c"],
    args = ["$(location //test/expect:hello.txt)", "hello"],
    data = ["//test/expect:hello.txt"],
)

#### pipem_test

cc_test(
    name = "pipem_test",
    srcs = ["pipem_test.c"],
    args = [
        "$(location //tool:shout)",
        "$(location //tool:expectish)",
    ],
    deps = ["//:lace_tool_lib"],
    data = ["//tool:expectish", "//tool:shout"],
)

#### spawn

spawn_test(
    name = "spawn_not_not_test",
    args = ["!", "!"],
)
spawn_test(
    name = "spawn_spawn_not_not_test",
    args = ["$(location //tool:spawn)", "!", "!"],
    data = ["//tool:spawn"],
)
spawn_test(
    name = "spawn_not_spawn_not_test",
    args = ["!", "$(location //tool:spawn)", "!"],
    data = ["//tool:spawn"],
)
spawn_test(
    name = "spawn_not_spawn_nothing_test",
    args = ["!", "$(location //tool:spawn)"],
    data = ["//tool:spawn"],
)

spawn_test(
    name = "spawn_not_spawn_absent_test",
    args = ["!", "$(location //tool:spawn)", "$(location //tool:spawn).doesnotexist"],
    data = ["//tool:spawn"],
)
spawn_test(
    name = "spawn_not_spawn_not_absent_test",
    args = ["!", "$(location //tool:spawn)", "!", "$(location //tool:spawn).doesnotexist"],
    data = ["//tool:spawn"],
)

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
    name = "spawn_status_test",
    srcs = ["spawn_status_test.c"],
    deps = ["//:lace_tool_lib"],
    args = ["$(location :success)", "$(location :failure)"],
    data = [
        ":failure",
        ":success",
    ],
)
