
def lace_expect_test(name, srcs, expect, data=[], args=[]):
  native.sh_test(
      name = name,
      srcs = ["//tool:exec.sh"],
      data = [
          expect,
          "//tool:comparispawn",
          "//:lace",
      ] + srcs + data,
      args = [
          "$(location //tool:comparispawn)",
          "$(location " + expect + ")",
          "$(location //:lace)",
          "-x",
          "$(location " + srcs[0] + ")",
      ] + args,
      size = "small",
  )
