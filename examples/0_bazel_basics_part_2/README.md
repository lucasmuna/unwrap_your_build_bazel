# Bazel Workspace, Commands and Targets

Support for https://unwrap.hashnode.dev/bazel-basics-part-2.

Get more familiarized with a Bazel workspace and its commands and targets.

## Play with some commands

You can run the following commands, in order, to become more familiarized with them and also with how Bazel works:

1. `bazel version`: Output Bazel version. We pinned Bazel to version `8.3.1` under [`.bazelversion`](./.bazelversion)
2. `bazel build //...`: Build all targets of this workspace. As Bazel cache is cold, it might take a few seconds.
3. `bazel build //...`: Build all targets of this workspace. As Bazel cache is warm, it will be really fast.
4. `bazel query //...`: Query all targets of this workspace. You will see all targets under [`cc_example`](./cc_example/BUILD) and [`py_example`](./py_example/BUILD) packages.
5. `bazel run //cc_example:example`: Run a cc target, in this case `//cc_example:example`. It should be fast because cache is warm.
6. `bazel run //py_example:example`: Run a python target, in this case `//py_example:example`. It should be fast because cache is warm.
7. `bazel test //...`: Test all targets of this workspace. It should be fast because cache is warm, and test should run.
8. `bazel test //...`: Test all targets of this workspace. It should be fast because cache is warm, and test should not run as they are also cached.
9. `bazel clean`: Deletes all output directories for this Bazel instance.
10. `bazel test //...`: Test all targets of this workspace. It might take a few seconds as cache is cold, and test should run.
11. `bazel clean --expunge`: Removes the entire output base tree for this Bazel instance.
12. `bazel build //cc_example:example -s` -> Build `//cc_example:example` and output the performed subcommands. As this is a cc target, you will see respective compilation command that was used to build the `cc_binary` target.

## Additional material

Official links with additional material:

- Commands and Options: https://bazel.build/docs/user-manual
- Command-line reference: https://bazel.build/reference/command-line-reference
- Query guide: https://bazel.build/query/guide
- Query language reference: https://bazel.build/query/language
- C/C++ rules reference: https://bazel.build/reference/be/c-cpp
- Python rules reference: https://bazel.build/reference/be/python
