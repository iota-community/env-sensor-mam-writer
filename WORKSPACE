load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "entangled",
    commit = "f0ab6522942ff02bc95f84d37ca1bd2a0a267dfe",
    remote = "https://github.com/iotaledger/entangled.git",
)

git_repository(
    name = "rules_iota",
    commit = "1cb59eea62fd1d071de213a9aa46e61e8273472d",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "nanopb",
    commit = "a2db482712a575ab01c608ec129c8a07454be0ef",
    remote = "https://github.com/nanopb/nanopb.git",
)

load("@rules_iota//:defs.bzl", "iota_deps")
iota_deps()