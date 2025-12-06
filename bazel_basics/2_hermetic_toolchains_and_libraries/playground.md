# Bazel Hermetic Toolchains and Libraries - Playground

## Play with some commands

You can run the following commands, in order, to become more familiarized with them and also with how Bazel works:

1. `bazel build //...`: Build all targets of this workspace and check that everyting works. It might take a while to download the hermetic toolchains in hte first run, specially the llvm one.
2. `bazel mod graph`: Output the current tree of bzlmod dependencies. You should see the ones stated under [`MODULE.bazel`](./MODULE.bazel) and also their's respective transitive dependencies.
3. `bazel test //...`: Test all targets of this workspace.
4. Remove the `googletest` dependency under [`cc_example/BUILD`](./cc_example/BUILD) and run `bazel test //cc_example:test`, it should fail because the hermetic toolchain does not have `googletest`. Add it back and run the same command again. The cache is also invalidated when you modify the target under the `BUILD` file.
5. Remove the `pytest` dependency under [`py_example/BUILD`](./py_example/BUILD) and run `bazel test //py_example:test`, it should fail because the hermetic toolchain does not have `pytest`. Add it back and run the same command again. The cache is also invalidated when you modify the target under the `BUILD` file.
6. `bazel clean`: Deletes all output directories for this Bazel instance.
7. `bazel test //...`: Test all targets of this workspace. The tests will not be cached, but the dependencies will already be warmed up, it will not need to fetch and extract packages.
8. `bazel clean --expunge`: Removes the entire output base tree for this Bazel instance.
9. `bazel test //...`: Test all targets of this workspace. Both tests and dependencies will not be cached, it will need to fetch and extract packages again.

## Additional material

Additional material:

- `googletest`: https://github.com/google/googletest
- `pypi`: https://pypi.org/project/pytest/
- `rules_foreing_cc`: https://github.com/bazel-contrib/rules_foreign_cc
- `rules_python`: https://github.com/bazel-contrib/rules_python
- `toolchains_llvm`: https://github.com/bazel-contrib/toolchains_llvm
