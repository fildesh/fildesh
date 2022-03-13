# Public Bazel rules/macros (except for the ones starting with underscores).
#
# This would be in @rules_fildesh//fildesh:defs.bzl
# if such a @rules_fildesh repository existed!


def _fildespawn_test_impl(ctx):
  executable = ctx.actions.declare_file(ctx.label.name)
  ctx.actions.symlink(output=executable,
                      target_file=ctx.file._fildespawn,
                      is_executable=True)
  runfiles = ctx.runfiles(files=ctx.files.data)
  return DefaultInfo(executable=executable, runfiles=runfiles)

_fildespawn_test = rule(
    implementation = _fildespawn_test_impl,
    test = True,
    attrs = {
        "data": attr.label_list(allow_files=True),
        "_fildespawn": attr.label(
            default = Label("//:fildespawn"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)

def fildespawn_test(name, size="small", **kwargs):
  _fildespawn_test(name=name, size=size, **kwargs)


def _fildesh_test_impl(ctx):
  executable = ctx.actions.declare_file(ctx.label.name)
  ctx.actions.symlink(output=executable,
                      target_file=ctx.file._fildesh,
                      is_executable=True)
  runfiles = ctx.runfiles(files=ctx.files.data)
  return DefaultInfo(executable=executable, runfiles=runfiles)

_fildesh_test = rule(
    implementation = _fildesh_test_impl,
    test = True,
    attrs = {
        "data": attr.label_list(allow_files=True),
        "_fildesh": attr.label(
            default = Label("//:fildesh"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)

def fildesh_test(
    name, srcs,
    main=None,
    aliases=[], named_args=[],
    data=[], args=[],
    forkonly=False,
    size="small"):

  # Windows has trouble with fildesh spawning itself when it's symlinked.
  alias_fildesh = True

  fildesh_options = []
  if alias_fildesh:
    fildesh_options += ["-alias", "fildesh=$(location //:fildesh)"]
    data = data + ["//:fildesh"]
  for a in aliases:
    fildesh_options += ["-alias", a]
  for a in named_args:
    fildesh_options += ["-a", a]

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

  _fildesh_test(
      name = name,
      args = (
          fildesh_options +
          ["-f", "$(location " + main + ")"] +
          args),
      data = srcs + data,
      size = size,
  )
