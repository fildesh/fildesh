
def spawn_test(name, data=[], args=[],
               expect_failure=False,
               size="small",
               **kwargs):
  status_args = []
  if expect_failure:
    status_args += ["!"]

  native.cc_test(
      name = name,
      srcs = ["@fildesh//tool:spawn.c"],
      data = data,
      args = status_args + args,
      size = size,
      **kwargs,
  )

def fildesh_failure_test(name, srcs, aliases=[], data=[], args=[],
                         forkonly=False,
                         size="small",
                         **kwargs):
  fildesh_options = []
  for a in aliases:
    fildesh_options += ["-alias", a]
  if forkonly:
    fildesh_options += ["-forkonly"]
  spawn_test(
      name = name,
      data = [
          "@fildesh//:fildesh",
      ] + srcs + data,
      args = [
          "$(location @fildesh//:fildesh)",
      ] + fildesh_options + [
          "-f", "$(location " + srcs[0] + ")",
      ] + args,
      expect_failure = True,
      size = size,
      **kwargs,
  )
