
alias(
    name = "lace",
    actual = "//src:lace",
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
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
)
