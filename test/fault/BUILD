load("//tool:spawn.bzl", "lace_test")

lace_test(
    name = "executable_bytes_of_test",
    srcs = ["executable_bytes_of_test.lace"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
    expect_failure = True,
)

lace_test(
    name = "false_test",
    srcs = ["false_test.lace"],
    expect_failure = True,
)

lace_test(
    name = "include_missing_test",
    srcs = ["include_missing_test.lace"],
    expect_failure = True,
)

lace_test(
    name = "pump_executable_bytes_oxf_test",
    srcs = ["pump_executable_bytes_oxf_test.lace"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
    expect_failure = True,
)

