load("//test/benchmark:benchmark.bzl", "cc_benchmark")


config_setting(
    name = "full_benchmarking",
    flag_values = {
        "//test:enable_full_benchmarking": "true",
    }
)

cc_benchmark(
    name = "file_write_read_benchmark",
    srcs = ["file_write_read_benchmark.cc"],
    deps = ["//:fildesh_lib"],
)

cc_benchmark(
    name = "grow_mpop_benchmark",
    srcs = ["grow_mpop_benchmark.cc"],
    deps = ["//:fildesh_lib"],
)

cc_benchmark(
    name = "map_benchmark",
    srcs = ["map_benchmark.cc"],
    deps = [
        "//src/cx:int_int_map_lib",
    ],
)
