load("//:def.bzl", "spawn_test")

spawn_test(
    name = "named_arg_test",
    binary = "//:fildesh",
    args = [
        "-a", "hello=Hello",
        "-a", "world=World",
        "-f", "$(location named_arg_test.fildesh)",
    ],
    data = ["named_arg_test.fildesh"],
)
