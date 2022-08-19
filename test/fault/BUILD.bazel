load("//tool:spawn.bzl", "fildesh_failure_test")

fildesh_failure_test(
    name = "executable_bytes_of_test",
    srcs = ["executable_bytes_of_test.fildesh"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
)

fildesh_failure_test(
    name = "false_test",
    srcs = ["false_test.fildesh"],
)

fildesh_failure_test(
    name = "include_missing_test",
    srcs = ["include_missing_test.fildesh"],
)

fildesh_failure_test(
    name = "pump_executable_bytes_oxf_test",
    srcs = ["pump_executable_bytes_oxf_test.fildesh"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
)

