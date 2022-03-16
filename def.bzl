# Public Bazel rules/macros (except for the ones starting with underscores).
#
# This would be in @rules_fildesh//fildesh:defs.bzl
# if such a @rules_fildesh repository existed!

def spawn_test(
    name,
    binary, args=[], data=[],
    expect_failure=False,
    size="small",
    **kwargs):
  spawn_args = []
  if expect_failure:
    spawn_args += ["!"]
  spawn_args += ["$(location " + binary + ")"]

  native.cc_test(
      name = name,
      srcs = ["@fildesh//tool:spawn.c"],
      args = spawn_args + args,
      data = [binary] + data,
      size = size,
      **kwargs)


def fildesh_test(
    name, srcs,
    main=None,
    aliases=[],
    named_inputs=[],
    data=[], args=[],
    forkonly=False,
    **kwargs):
  data = list(data)
  fildesh_options = []

  # Windows has trouble with fildesh spawning itself when it's symlinked.
  alias_fildesh = True
  if alias_fildesh:
    fildesh_options += ["-alias", "fildesh=$(location @fildesh//:fildesh)"]

  if type(aliases) == type([]):
    for a in aliases:
      fildesh_options += ["-alias", a]
  else:
    for (k, v) in sorted(aliases.items()):
      fildesh_options += ["-alias", k + "=$(location " + v + ")"]
      data.append(v)

  if type(named_inputs) == type([]):
    for a in named_inputs:
      fildesh_options += ["-a", a]
  else:
    for (k, v) in sorted(named_inputs.items()):
      fildesh_options += ["-a", k + "=$(location " + v + ")"]
      data.append(v)

  if forkonly:
    fildesh_options += ["-forkonly"]

  if not main:
    if len(srcs) == 1:
      main = srcs[0]
    else:
      for src in srcs:
        if src.endswith(name + ".fildesh"):
          main = src
  if not main:
    fail("unknown main source file")

  spawn_test(
      name = name,
      binary = "@fildesh//:fildesh",
      args = fildesh_options + [
          "-f", "$(location " + main + ")",
      ] + args,
      data = srcs + data,
      **kwargs)
