
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
            default = Label("//tool:fildespawn"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)

def fildespawn_test(name, args, data=[], size="small"):
  # Example of how to write a test rule, but it's missing coverage!
  _fildespawn_test(name=name, args=args, data=data, size=size)

