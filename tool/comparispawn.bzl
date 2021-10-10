load("@fildesh//tool:spawn.bzl", "spawn_test")

def fildesh_expect_test(name, srcs, expect,
                        data=[], args=[], size="small",
                        **kwargs):
  spawn_test(
      name = name,
      data = [
          expect,
          "@fildesh//tool:comparispawn",
          "@fildesh//:fildesh",
      ] + srcs + data,
      args = [
          "$(location @fildesh//tool:comparispawn)",
          "$(location " + expect + ")",
          "$(location @fildesh//:fildesh)",
          "-x",
          "$(location " + srcs[0] + ")",
      ] + args,
      size = size,
      **kwargs,
  )
