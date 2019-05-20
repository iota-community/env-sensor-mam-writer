package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "app",
    srcs = ["server-client.c"],
    deps = [
        "//encode:encode",
        "//logging:logging",
        "//proto_compiled:proto_compiled",
        "//sensor:sensor",
        "@nanopb//:nanopb",
        "//iota:mam",
    ],
    copts = ["-DLOGGER_ENABLE"],
    linkopts = ["-pthread"],
)
