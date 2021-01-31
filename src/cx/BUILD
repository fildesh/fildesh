
cc_library(
    name = "cx_lib",
    srcs = [
        "alphatab.c",
        "alphatab.h",
        "associa.h",
        "bittable.h",
        "bstree.c",
        "bstree.h",
        "def.h",
        "fileb.c",
        "fileb.h",
        "lgtable.h",
        "ofile.c",
        "ofile.h",
        "ospc.c",
        "ospc.h",
        "rbtree.c",
        "rbtree.h",
        "synhax.h",
        "syscx.c",
        "syscx.h",
        "table.h",
        "urandom.c",
        "urandom.h",
        "xfile.c",
        "xfile.h",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "bittable_test",
    srcs = ["bittable_test.c"],
    deps = [":cx_lib"],
    size = "small",
)

cc_test(
    name = "fileb_test",
    srcs = ["fileb_test.c"],
    deps = [":cx_lib"],
    size = "small",
)

cc_test(
    name = "lgtable_test",
    srcs = ["lgtable_test.c"],
    deps = [":cx_lib"],
    size = "small",
)

cc_test(
    name = "ospc_test",
    srcs = ["ospc_test.c"],
    deps = [":cx_lib"],
    size = "small",
)

cc_test(
    name = "rbtree_test",
    srcs = ["rbtree_test.c"],
    deps = [":cx_lib"],
    size = "small",
)

cc_test(
    name = "table_test",
    srcs = ["table_test.c"],
    deps = [":cx_lib"],
    size = "small",
)
