load("//tool/bazel:spawn.bzl", "spawn_test")


def fildesh_failure_test(name, srcs, input_by_option=None):
  args = ["-f", "$(location " + srcs[0] + ")"]
  data = list(srcs)
  for k in sorted(input_by_option or {}):
    v = input_by_option[k]
    args += ["--" + k + "=$(location " + v + ")"]
    data += [v]
  spawn_test(
      name = name,
      expect_failure = True,
      binary = "@fildesh//:fildesh",
      data = data,
      args = args,
  )
