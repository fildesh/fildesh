
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

def fildespawn_test(size="small", **kwargs):
  _fildespawn_test(size=size, **kwargs)
