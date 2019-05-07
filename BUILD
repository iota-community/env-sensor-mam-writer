package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "app",
    copts = ["-DLOGGER_ENABLE"],
    srcs = ["server-client.c"],
    deps = ["//app/proto_compiled:proto_compiled"],
    visibility = ["//visibility:public"],
)