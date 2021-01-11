
def lace_expect_test(name, srcs, expect, data=[], args=[]):
  native.sh_test(
      name = name,
      srcs = ["//src:exec.sh"],
      data = [
          expect,
          "//src:comparispawn",
          "//src:lace",
      ] + srcs + data,
      args = [
          "$(location //src:comparispawn)",
          "$(location " + expect + ")",
          "$(location //src:lace)",
          "-x",
          "$(location " + srcs[0] + ")",
      ] + args,
  )
