load("//tool/bazel:spawn.bzl", "spawn_test")

def fildesh_expect_test(name, srcs, expect,
                        data=[], args=[],
                        **kwargs):
  spawn_test(
      name = name,
      binary = "@fildesh//tool:comparispawn",
      args = [
          "$(location " + expect + ")",
          "$(location @fildesh//:fildesh)",
          "-x",
          "$(location " + srcs[0] + ")",
      ] + args,
      data = [
          expect,
          "@fildesh//:fildesh",
      ] + srcs + data,
      **kwargs,
  )
