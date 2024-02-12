load("//tool/bazel:spawn_test.bzl", "spawn_test")

def fildesh_expect_test(name, srcs, expect,
                        data=[], args=[],
                        forkonly=False,
                        **kwargs):
  fildesh_options = []
  if forkonly:
    fildesh_options += ["-forkonly"]
  spawn_test(
      name = name,
      binary = "//tool:comparispawn",
      args = [
          "$(location " + expect + ")",
          "$(location //tool:fildesh)",
      ] + fildesh_options + [
          "-f", "$(location " + srcs[0] + ")",
      ] + args,
      data = [
          expect,
          "//tool:fildesh",
      ] + srcs + data,
      **kwargs,
  )
