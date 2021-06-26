load("//tool:spawn.bzl", "spawn_test")

def lace_expect_test(name, srcs, expect,
                     data=[], args=[], size="small",
                     **kwargs):
  spawn_test(
      name = name,
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
      size = size,
      **kwargs,
  )
