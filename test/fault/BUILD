load("//tool:spawn.bzl", "fildesh_test")

fildesh_test(
    name = "executable_bytes_of_test",
    srcs = ["executable_bytes_of_test.fildesh"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
    expect_failure = True,
)

fildesh_test(
    name = "false_test",
    srcs = ["false_test.fildesh"],
    expect_failure = True,
)

fildesh_test(
    name = "include_missing_test",
    srcs = ["include_missing_test.fildesh"],
    expect_failure = True,
)

fildesh_test(
    name = "pump_executable_bytes_oxf_test",
    srcs = ["pump_executable_bytes_oxf_test.fildesh"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
    expect_failure = True,
)

