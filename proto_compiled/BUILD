package(default_visibility = ["//visibility:public"])

cc_library(
    name = "proto_compiled",
    visibility = ["//visibility:public"],
    srcs = ["DataRequest.pb.c", "DataResponse.pb.c", "FeatureResponse.pb.c"],
    hdrs = ["DataRequest.pb.h", "DataResponse.pb.h", "FeatureResponse.pb.h"],
    deps = [
        "@nanopb//:nanopb",
    ],
)