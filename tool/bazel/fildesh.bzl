load("//tool/bazel:spawn.bzl", "spawn_test")

def fildesh_test(
    name, srcs,
    main=None,
    aliases=[],
    named_inputs=[],
    data=[], args=[],
    fildesh_options=[],
    forkonly=False,
    expect_failure=False,
    **kwargs):
  """Fildesh test macro.

  All kwargs get passed to a cc_test(),
  so you can also use options like `env={"K": "V"}`.
  """
  data = list(data)
  fildesh_options = list(fildesh_options)

  if expect_failure:
    fail("Please test exit statuses with the expect_failure Fildesh builtin " +
        "instead of expecting the entire test script to fail.")

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
      args = [
          "-tmpdir_from_env", "TEST_TMPDIR",
      ] + fildesh_options + [
          "-f", "$(location " + main + ")",
      ] + args,
      data = srcs + data,
      **kwargs)
