load("//tool/bazel:spawn.bzl", "spawn_test")


def fildesh_failure_test(name, srcs, aliases=[], data=[], args=[],
                         forkonly=False,
                         **kwargs):
  fildesh_options = []
  for a in aliases:
    fildesh_options += ["-alias", a]
  if forkonly:
    fildesh_options += ["-forkonly"]
  spawn_test(
      name = name,
      expect_failure = True,
      binary = "@fildesh//:fildesh",
      data = srcs + data,
      args = fildesh_options + [
          "-f", "$(location " + srcs[0] + ")",
      ] + args,
      **kwargs,
  )
