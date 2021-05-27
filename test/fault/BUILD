
cc_test(
    name = "false_test",
    srcs = [
        "//tool:spawn.c",
    ],
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
