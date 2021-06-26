
alias(
    name = "lace",
    actual = "//src:lace",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "lace_compat_lib",
    srcs = [
        "//compat:fd.c",
        "//include:lace_compat_fd.h",
    ],
    includes = ["include"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "lace_lib",
    srcs = [
        "//include:lace.h",
        "//src:infile.c",
        "//src:instream.c",
        "//src:kve.c",
        "//src:kve.h",
        "//src:outfile.c",
        "//src:outstream.c",
        "//src:string.c",
    ],
    includes = ["include"],
    deps = [":lace_compat_lib"],
    visibility = ["//visibility:public"],
)


cc_library(
    name = "lace_tool_lib",
    srcs = [
        "//include:lace_posix_thread.h",
        "//include:lace_tool.h",
        "//tool:argspawn.c",
        "//tool:expectish.c",
        "//tool:pipem.c",
        "//tool:shout.c",
        "//tool:spawn.c",
    ],
    defines = ["LACE_TOOL_LIBRARY"],
    includes = ["include"],
    deps = [":lace_compat_lib"],
    linkopts = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    visibility = ["//visibility:public"],
)
