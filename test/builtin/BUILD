load("//src:comparispawn.bzl", "lace_expect_test")

lace_expect_test(
    name = "elastic_cat_test",
    srcs = ["elastic_cat.lace"],
    expect = ":hello_newline.txt",
)

lace_expect_test(
    name = "elastic_tee_test",
    srcs = ["elastic_tee.lace"],
    expect = ":hello_newline_x2.txt",
)

lace_expect_test(
    name = "xpipe_nested_test",
    srcs = ["xpipe_nested.lace"],
    expect = "//test/expect:xpipe_nested.txt",
)

lace_expect_test(
    name = "zec_cat_test",
    srcs = ["zec_cat.lace"],
    expect = ":hello_newline.txt",
)

