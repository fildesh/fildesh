load("//tool:spawn.bzl", "spawn_test")

spawn_test(
    name = "false_test",
    args = [
        "!",
        "$(location //:lace)",
        "$(location :false_test.lace)",
    ],
    data = [
        "false_test.lace",
        "//:lace",
    ],
)
