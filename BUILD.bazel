
alias(
    name = "fildesh",
    actual = "//src:fildesh",
    visibility = ["//visibility:public"],
)

alias(
    name = "fildespawn",
    actual = "//tool:fildespawn",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "fildesh_compat_lib",
    srcs = [
        "//compat:errno.c",
        "//compat:fd.c",
        "//compat:fd_exclusive.h",
        "//compat:file.c",
        "//compat:random.c",
        "//compat:sh.c",
        "//compat:string.c",
        "//include:fildesh_compat_errno.h",
        "//include:fildesh_compat_fd.h",
        "//include:fildesh_compat_file.h",
        "//include:fildesh_compat_random.h",
        "//include:fildesh_compat_sh.h",
        "//include:fildesh_compat_string.h",
    ],
    includes = ["include"],
    linkopts = select({
        "@platforms//os:windows": [
            "-DEFAULTLIB:advapi32.lib",  # For RtlGenRandom().
        ],
        "//conditions:default": ["-pthread"],
    }),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "fildesh_lib",
    srcs = [
        "//include:fildesh.h",
        "//src:alloc.c",
        "//src:infile.c",
        "//src:instream.c",
        "//src:kve.c",
        "//src:kve.h",
        "//src:log.c",
        "//src:mascii.c",
        "//src:mascii.h",
        "//src:outfile.c",
        "//src:outstream.c",
        "//src:string.c",
    ],
    includes = ["include"],
    deps = [":fildesh_compat_lib"],
    visibility = ["//visibility:public"],
)


cc_library(
    name = "fildesh_tool_lib",
    srcs = [
        "//include:fildesh_posix_thread.h",
        "//include:fildesh_tool.h",
        "//tool:pipem.c",
    ],
    includes = ["include"],
    deps = [":fildesh_compat_lib"],
    visibility = ["//visibility:public"],
)
