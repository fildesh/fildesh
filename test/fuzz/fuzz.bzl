
def cc_smoke_test(name, srcs, deps):
  srcs = srcs + [
      "//test/fuzz:fuzz_common.h",
      "//test/fuzz:smoke_common.h",
  ]
  native.cc_test(
      name = name,
      srcs = srcs,
      deps = deps,
      size = "small",
  )

def cc_fuzz_test(name, srcs, deps, max_guesses):
  srcs = srcs + [
      "//test/fuzz:fuzz_common.h",
  ]
  libfuzzer_compile_flags = ["-fsanitize=address,fuzzer"]
  native.cc_test(
      name = name,
      args = select({
          "//test:full_fuzzing_on": ["-runs=" + str(max_guesses)],
          "//conditions:default": [],
      }),
      srcs = select({
          "//test:full_fuzzing_on": srcs,
          "//conditions:default": srcs + ["//test/fuzz:fuzz_main.c"],
      }),
      deps = deps,
      size = "small",
      copts = select({
          "//test:full_fuzzing_on": libfuzzer_compile_flags,
          "//conditions:default": [],
      }),
      linkopts = select({
          "//test:full_fuzzing_on": libfuzzer_compile_flags,
          "//conditions:default": [],
      }),
  )
  native.cc_binary(
      name = name + "_full",
      srcs = srcs,
      deps = deps,
      copts = libfuzzer_compile_flags,
      linkopts = libfuzzer_compile_flags,
      testonly = 1,
      target_compatible_with = select({
          "//test:full_fuzzing_on": [],
          "//conditions:default": ["@platforms//:incompatible"],
      })
  )
