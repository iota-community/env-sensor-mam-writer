package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mam",
    srcs = ["common.c", "send-common.c", "send-header.c", "send-msg.c"],
    hdrs = ["common.h", "send-common.h", "send-msg.h"],
    deps = [
        "//logging:logging",
        "@entangled//mam/api",
        "@entangled//cclient/api",
        "@entangled//utils:time",
        "@entangled//common:defs",
        "@entangled//common/trinary:tryte",
        "@entangled//common/trinary:tryte_ascii",
    ],
)
