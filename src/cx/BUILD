
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
        "lgtable.h",
        "rbtree.c",
        "rbtree.h",
        "synhax.h",
        "syscx.c",
        "syscx.h",
        "table.h",
    ],
    deps = [
        "//:lace_lib",
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
    name = "lgtable_test",
    srcs = ["lgtable_test.c"],
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

cc_test(
    name = "tmpdir_test",
    srcs = ["tmpdir_test.c"],
    deps = [":cx_lib"],
    size = "small",
)
