load("//test/fuzz:fuzz.bzl", "cc_fuzz_test")


config_setting(
    name = "full_fuzzing",
    flag_values = {
        "//test:enable_full_fuzzing": "true",
    }
)


cc_fuzz_test(
    name = "grow_mpop_fuzz_test",
    srcs = ["grow_mpop_fuzz_test.c"],
    deps = ["//:lace_lib"],
)
