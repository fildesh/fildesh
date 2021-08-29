load("@rules_fuzzing//fuzzing:cc_defs.bzl", wrapped_cc_fuzz_test = "cc_fuzz_test")

def cc_fuzz_test(name, srcs, deps):
  wrapped_cc_fuzz_test(
      name = name,
      srcs = srcs + ["//test/fuzz:fuzz_common.c"],
      deps = deps,
      target_compatible_with = select({
          "//test/fuzz:full_fuzzing": [],
          "//conditions:default": ["@platforms//os:linux"],
      })
  )
