cc_binary(
    name = "example",
    srcs = ["example.cpp"],
    deps = [":lib"], # The `:` from `:lib` equals to `//cc_example:` in this case.
)

cc_library(
    name = "lib",
    hdrs = ["lib.h"],
)

cc_test(
    name = "test",
    srcs = ["test.cpp"],
    deps = [":lib"],
)
