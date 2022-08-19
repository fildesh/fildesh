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
    deps = ["//:fildesh_lib"],
    max_guesses = 50000,
)

cc_fuzz_test(
    name = "mascii_fuzz_test",
    srcs = ["mascii_fuzz_test.c"],
    deps = ["//:fildesh_lib"],
    max_guesses = 1000000,
)
