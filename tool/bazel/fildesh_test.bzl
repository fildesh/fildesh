load("//tool/bazel:spawn_test.bzl", _spawn_test = "spawn_test")


def fildesh_test(
    name, srcs,
    main=None,
    input_by_option=None,
    tool_by_alias=None,
    args=[],
    data=[],
    fildesh_options=[],
    forkonly=False,
    expect_failure=False,
    **kwargs):
  """Fildesh test macro.

  All kwargs get passed to a cc_test(),
  so you can also use options like `env={"K": "V"}`.
  """
  tool_aliases = sorted(tool_by_alias or dict())
  input_names = sorted(input_by_option or dict())
  args = list(args)
  data = list(data)
  fildesh_options = list(fildesh_options)

  if expect_failure:
    fail("Please test exit statuses with the expect_failure Fildesh builtin " +
        "instead of expecting the entire test script to fail.")

  if not main:
    if len(srcs) == 1:
      main = srcs[0]
  if not main:
    for src in srcs:
      if src.endswith(name + ".fildesh"):
        main = src
        break
  if not main:
    fail("unknown main source file")

  if forkonly:
    fildesh_options += ["-forkonly"]

  # Windows has trouble with fildesh spawning itself when it's symlinked.
  alias_fildesh = True
  if alias_fildesh:
    fildesh_options += ["-alias", "fildesh=$(location @fildesh//tool:fildesh)"]

  for k in tool_aliases:
    v = tool_by_alias[k]
    fildesh_options += ["-alias", k + "=$(location " + v + ")"]
    data += [v]

  for k in input_names:
    v = input_by_option[k]
    args += ["--" + k + "=$(location " + v + ")"]
    data += [v]

  _spawn_test(
      name = name,
      binary = "@fildesh//tool:fildesh",
      args = [
          "-tmpdir_from_env", "TEST_TMPDIR",
      ] + fildesh_options + [
          "-f", "$(location " + main + ")",
      ] + args,
      data = srcs + data,
      **kwargs)
