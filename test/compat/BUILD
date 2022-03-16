
cc_test(
    name = "fd_move_off_stdio_test",
    srcs = ["fd_move_off_stdio_test.c"],
    deps = ["//:fildesh_compat_lib"],
)

cc_test(
    name = "random_bytes_test",
    srcs = ["random_bytes_test.c"],
    deps = ["//:fildesh_compat_lib"],
)

cc_test(
    name = "sh_escape_argv_test",
    srcs = [
        "sh_escape_argv_test.c",
        "//tool:spawn.c",
    ],
    defines = ["FILDESH_TOOL_LIBRARY"],
    deps = ["//:fildesh_compat_lib"],
)

cc_test(
    name = "string_byte_translate_test",
    srcs = ["string_byte_translate_test.c"],
    deps = ["//:fildesh_compat_lib"],
)
