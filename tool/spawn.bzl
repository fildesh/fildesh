
def spawn_test(name, data=[], args=[], size="small", **kwargs):
  native.cc_test(
      name = name,
      srcs = ["@lace//tool:spawn.c"],
      data = data,
      args = args,
      size = size,
      **kwargs,
  )

def lace_test(name, srcs, aliases=[], data=[], args=[], size="small", **kwargs):
  alias_args = []
  for a in aliases:
    alias_args += ["-alias", a]
  spawn_test(
      name = name,
      data = [
          "@lace//:lace",
      ] + srcs + data,
      args = [
          "$(location @lace//:lace)",
      ] + alias_args + [
          "-f", "$(location " + srcs[0] + ")",
      ] + args,
      size = size,
      **kwargs,
  )
