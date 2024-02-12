
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
