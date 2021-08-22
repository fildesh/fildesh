
def spawn_test(name, data=[], args=[],
               expect_failure=False,
               size="small",
               **kwargs):
  status_args = []
  if expect_failure:
    status_args += ["!"]

  native.cc_test(
      name = name,
      srcs = ["@lace//tool:spawn.c"],
      data = data,
      args = status_args + args,
      size = size,
      **kwargs,
  )

def lace_test(name, srcs, aliases=[], data=[], args=[],
              expect_failure=False,
              forkonly=False,
              size="small",
              **kwargs):
  lace_options = []
  for a in aliases:
    lace_options += ["-alias", a]
  if forkonly:
    lace_options += ["-forkonly"]
  spawn_test(
      name = name,
      data = [
          "@lace//:lace",
      ] + srcs + data,
      args = [
          "$(location @lace//:lace)",
      ] + lace_options + [
          "-f", "$(location " + srcs[0] + ")",
      ] + args,
      expect_failure = expect_failure,
      size = size,
      **kwargs,
  )
