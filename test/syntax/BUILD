load("//tool:spawn.bzl", "lace_test")

lace_test(
    name = "concatenate_oooxfxax_test",
    srcs = ["concatenate_oooxfxax_test.lace"],
)
lace_test(
    name = "concatenate_forkonly_oooxfxax_test",
    srcs = ["concatenate_oooxfxax_test.lace"],
    forkonly = True,
)

lace_test(
    name = "executable_bytes_oxf_test",
    srcs = ["executable_bytes_oxf_test.lace"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
)
lace_test(
    name = "executable_bytes_ofxf_test",
    srcs = ["executable_bytes_ofxf_test.lace"],
    args = ["$(location //builtin:zec)"],
    data = ["//builtin:zec"],
)

lace_test(
    name = "pump_ofxf_test",
    srcs = ["pump_ofxf_test.lace"],
)
lace_test(
    name = "pump_oxf_test",
    srcs = ["pump_oxf_test.lace"],
)
lace_test(
    name = "pump_ofx_test",
    srcs = ["pump_ofx_test.lace"],
)
lace_test(
    name = "pump_ox_test",
    srcs = ["pump_ox_test.lace"],
)
