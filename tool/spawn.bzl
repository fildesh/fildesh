
def spawn_test(name, data=[], args=[], size="small", **kwargs):
  native.cc_test(
      name = name,
      srcs = [
          "//tool:spawn.c",
      ],
      data = data,
      args = args,
      size = size,
      **kwargs,
  )

def lace_test(name, srcs, data=[], args=[], size="small", **kwargs):
  spawn_test(
      name = name,
      data = [
          "//:lace",
      ] + srcs + data,
      args = [
          "$(location //:lace)", "-x", "$(location " + srcs[0] + ")",
      ] + args,
      size = size,
      **kwargs,
  )
