load("@lace//tool:spawn.bzl", "spawn_test")

def lace_expect_test(name, srcs, expect,
                     data=[], args=[], size="small",
                     **kwargs):
  spawn_test(
      name = name,
      data = [
          expect,
          "@lace//tool:comparispawn",
          "@lace//:lace",
      ] + srcs + data,
      args = [
          "$(location @lace//tool:comparispawn)",
          "$(location " + expect + ")",
          "$(location @lace//:lace)",
          "-x",
          "$(location " + srcs[0] + ")",
      ] + args,
      size = size,
      **kwargs,
  )
