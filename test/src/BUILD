load("//tool:spawn.bzl", "spawn_test")

spawn_test(
    name = "chatty_test",
    args = ["$(location //src:chatty)"],
    data = ["//src:chatty"],
    target_compatible_with = [
        "@platforms//os:linux",
    ],
)

cc_test(
    name = "delimit_edge_test",
    srcs = ["delimit_edge_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "getline_test",
    srcs = ["getline_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "gets_test",
    srcs = ["gets_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "kve_test",
    srcs = [
        "kve_test.c",
    ],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "parse_int_test",
    srcs = ["parse_int_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "puts_test",
    srcs = ["puts_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "read_file_lines_test",
    data = ["read_file_lines_test.c"],
    srcs = ["read_file_lines_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "sibling_file_test",
    srcs = ["sibling_file_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "size_test",
    srcs = ["size_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "skipstr_test",
    srcs = ["skipstr_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)

cc_test(
    name = "slicechrs_test",
    srcs = ["slicechrs_test.c"],
    deps = ["//:fildesh_lib"],
    size = "small",
)
