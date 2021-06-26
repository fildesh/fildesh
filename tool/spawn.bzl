
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
