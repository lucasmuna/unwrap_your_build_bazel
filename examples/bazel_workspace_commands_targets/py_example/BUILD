py_binary(
    name = "example",
    srcs = ["example.py"],
    deps = [":lib"], # The `:` from `:lib` equals to `//py_example:` in this case.
)

py_library(
    name = "lib",
    srcs = ["lib.py"],
)

py_test(
    name = "test",
    srcs = ["test.py"],
    deps = [":example"], # It implicitly depends on `:lib` as well.
)
