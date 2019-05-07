package(default_visibility = ["//visibility:public"])

cc_library(
    name = "encode",
    srcs = ["encode.c", "decode.c"],
    hdrs = ["encode.h", "decode.h"],
    deps = [
        "//logging:logging",
        "@nanopb//:nanopb",
        "//proto_compiled:proto_compiled",
    ],
    linkopts = ["-pthread"],
)
