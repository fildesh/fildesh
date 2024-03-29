
def cc_benchmark(name, srcs, deps=[], **kwargs):
  native.cc_test(
      name = name,
      srcs = srcs,
      deps = deps + [
          "@google_benchmark//:benchmark_main",
      ],
      args = select({
          "//test:full_benchmarking_on": [],
          "//conditions:default": ["--benchmark_min_time=0.000001"],
      }),
      **kwargs,
  )
