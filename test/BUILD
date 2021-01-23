load("//src:comparispawn.bzl", "lace_expect_test")

lace_expect_test(
    name = "args_test",
    srcs = ["//example:args.lace"],
    expect = "//test/expect:args.txt",
)

lace_expect_test(
    name = "args2_test",
    srcs = ["//example:args.lace"],
    expect = "//test/expect:args2.txt",
    args = [
        "2",
        "7",
    ],
)

lace_expect_test(
    name = "convoluted_test",
    srcs = ["//example:convoluted.lace"],
    expect = "//test/expect:convoluted.txt",
)

lace_expect_test(
    name = "cycle_test",
    srcs = ["//example:cycle.lace"],
    expect = "//test/expect:cycle.txt",
)

lace_expect_test(
    name = "familiar_test",
    srcs = ["//example:familiar.lace"],
    expect = "//test/expect:familiar.txt",
)

lace_expect_test(
    name = "hello_test",
    srcs = ["//example:hello.lace"],
    expect = "//test/expect:hello.txt",
)

lace_expect_test(
    name = "name_test",
    srcs = [
        "//example:name.lace",
        "//example:includetest.lace",
    ],
    expect = "//test/expect:name.txt",
)

lace_expect_test(
    name = "silly_test",
    srcs = ["//example:silly.lace"],
    expect = "//test/expect:silly.txt",
    data = [
        "//example:args.lace",
        "//example:cycle.lace",
        "//example:hello.lace",
    ],
)

lace_expect_test(
    name = "test_test",
    srcs = ["//example:test.lace"],
    expect = "//test/expect:test.txt",
)

lace_expect_test(
    name = "xpipe_nested_test",
    srcs = ["xpipe_nested.lace"],
    expect = "//test/expect:xpipe_nested.txt",
)
