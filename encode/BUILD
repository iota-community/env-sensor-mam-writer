package(default_visibility = ["//visibility:public"])

cc_library(
    name = "encode",
    srcs = ["encode.c", "decode.c"],
    hdrs = ["encode.h", "decode.h"],
    deps = [
        "//logging:logging",
    ],
    linkopts = ["-pthread"],
)
