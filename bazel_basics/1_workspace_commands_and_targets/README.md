# Workspace, Commands, and Targets

If you're here, you've probably decided to give Bazel a shot. As developers, we often prefer to learn by doing rather than by reading tons of documentation. So, let's dive into some examples together. The idea is to get more familiar with a Bazel workspace, its commands, and its targets.

**Note:** I won’t be covering `WORKSPACE` files, as they are being deprecated. Starting with Bazel 9, this older dependency management system will be completely removed in favor of `MODULE.bazel`.

**Heads up:** We don't have a hermetic setup yet, which means Bazel will use your computer's existing toolchains. If you don't have a C++ toolchain installed, it's best to stick with the Python examples for now.

## Playground

If you want to play with some examples directly, check out [playground.md](./playground.md).

## Bazel Workspace

I can't describe it better than the [official documentation](https://bazel.build/concepts/build-ref#workspace), so here it is:

> A _workspace_ is the environment shared by all Bazel commands run from the same main repo.

> Note that historically the concepts of "repository" and "workspace" have been conflated; the term "workspace" has often been used to refer to the main repository, and sometimes even used as a synonym of "repository".

So, what is a Bazel repository? It's simply a source tree with a `MODULE.bazel` file at its root. The main repository is usually your current project or Git repository, which can depend on other external repositories (each with its own `MODULE.bazel` file).

From this point forward, we'll consider that a defined set of repositories—the main one plus any external ones—comprises the workspace.

Let's create a minimal workspace and test it out. In a new directory, do the following:

```bash
touch MODULE.bazel
touch BUILD
bazel build //...
```

As you can see, with two empty files, you already have a working Bazel workspace.

### Workspace files

I won’t go into the details of every possible file, as that would be both boring and unproductive. However, the following list summarizes the most useful ones:

- Loaded from another repository:
  - **MODULE.bazel**: This file defines a Bazel repository as a [Bazel module](https://bazel.build/external/module). It sets the module's name, version, and lists its dependencies on other modules.
  - **BUILD:** Defines a [Bazel package](https://bazel.build/concepts/build-ref#packages) using Starlark code. It may contain multiple Bazel targets.
  - `*.bzl` **files**: Extend Bazel using Starlark code. Check [the documentation](https://bazel.build/rules/lib/globals/bzl) for how to write and use these files.
- Not loaded from external repositories:
  - **.bazelrc:** This is a [Bazel configuration file](https://bazel.build/run/bazelrc) that helps you manage and maintain multiple build configurations, making your builds more consistent.
  - **.bazelversion:** A simple, single-line file that specifies the exact Bazel version to be used for the workspace, like `8.3.1`. This ensures everyone on the team uses the same version.
  - **.bazelignore:** Similar to `.gitignore`, this tells Bazel which directories to ignore.

Note that I’ve split the files into two categories. Configuration files like `.bazelrc` are not inherited from external dependencies, as they are specific to the workspace. In contrast, files that define a repository, like `BUILD` and `.bzl` files, are transitively loaded and inherited by dependent workspaces.

After this brief introduction to workspace files, let’s complete our setup by adding some configurations.

```bash
cat > MODULE.bazel << EOL
module(
    name = "bazel_workspace_commands_targets",
    version = "0.0.1",
)
EOL
```

```bash
echo "common --lockfile_mode=off" >> .bazelrc
echo "8.3.1" >> .bazelversion
touch .bazelignore
```

## Targets

To avoid getting bogged down in details, think of a [_Bazel target_](https://bazel.build/extending/rules#target_instantiation) as something you want to build or run. To create a target, you use a [_Bazel rule_](https://bazel.build/extending/rules), which is like a recipe for building it. For example, to create a C++ binary, you use the `cc_binary` rule.

All instantiated targets can be referenced using Bazel commands. However, Bazel doesn't use file paths; it uses [_labels_](https://bazel.build/concepts/labels). For instance:

- If you define a target named `our_target` in the root `BUILD` file, its label is `//:our_target`.
- If you define the same target in `our_package/BUILD`, its label becomes `//our_package:our_target`.

There are also [special target patterns](https://bazel.build/query/language#target-patterns) like `//...`, which are quite handy for running or building everything recursively.

Anyway, let’s not worry about all the details just yet. Let's jump into some example code. I’ve prepared two sets of examples, [one for C++](./cc_example) and [one for Python](./py_example). Each includes a binary, a library, and a test.

### C++ Example Target

Let's start by creating our C++ code:

```bash
mkdir cc_example
cd cc_example

cat > example.cpp << EOL
#include "lib.h"
int main() {
    printMessage("Unwrap Your Build");
    return 0;
}
EOL

cat > lib.h << EOL
#include <iostream>
#include <string>
void printMessage(const std::string& message) {
    std::cout << message << std::endl;
}
EOL

cat > test.cpp << EOL
#include "lib.h"
int main() {
    printMessage("Unwrap Your Build");
    return 0;
}
EOL
```

With the source code in place, we just need to tell Bazel how to build it. To do this, we mark `cc_example` as a package by adding a `BUILD` file and then use rules like `cc_binary`, `cc_library`, and `cc_test` to define the targets for this package.

```bash
cat > BUILD << EOL
cc_binary(
    name = "example",
    srcs = ["example.cpp"],
    deps = [":lib"],
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
EOL
```

At this point, we have proper Bazel targets for our C++ code, which means we can use Bazel commands like `build`, `run`, and `test` on them. The following list summarizes these commands:

- `bazel build //cc_example/...`: Builds all targets under the `cc_example` package.
- `bazel run //cc_example:example`: Runs the binary target named `example`.
- `bazel test //cc_example/...`: Executes all tests under the `cc_example` package.

Note that it's not necessary to build before running; Bazel automatically builds any required targets. Also, if you run the same commands again, Bazel will try to use cached results, so repeated actions will be much faster.

### Python Example Target

The Python example is very similar to the C++ one. The main differences are:

- The source code, of course, as we’re writing Python now.
- The rules used to instantiate targets start with `py_` instead of `cc_`.

That’s it! Everything else remains the same. You can run the same Bazel commands—just don't forget to change the target labels.

This is great because it provides an abstraction layer over very different programming languages, allowing you to build and run them in a consistent way.

I won’t include code blocks for this section, but you should definitely try it out yourself. Check out the [Python example](./py_example) in this repository.

## Commands

We’ve already covered some basic Bazel commands in the previous sections. While Bazel has only a few core commands, it offers thousands of options that modify their behavior. Be sure to check the [official documentation](https://bazel.build/reference/command-line-reference) whenever you need to customize a command.

For now, let’s focus on five main commands:

- `bazel build`: Builds one or more targets.
  - You don’t need to build explicitly before running or testing; Bazel knows what needs to be built automatically.
  - Bazel also caches built artifacts by default.
  - Every target can be built.
- `bazel run`: Runs a specific executable target, usually created with `*_binary` rules.
- `bazel test`: Runs one or more test targets, usually created with `*_test` rules.
  - Bazel caches successful test results by default.
  - Bazel also offers a `coverage` command for test coverage reports.
- `bazel query`: Queries one or more targets.
  - Queries can return labels, configurations, locations, and other useful information—great for debugging or extracting details about your targets.
  - There are three types of queries: `query`, `cquery`, and `aquery`.
  - Check the Bazel query [guide](https://bazel.build/query/guide) and [reference](https://bazel.build/query/language) for more details.
- `bazel clean`: Deletes all output directories for the current Bazel instance.
  - Add the `--expunge` flag to remove the entire output base tree.
  - This command is more common for build system developers and shouldn't be part of a regular developer's daily workflow, especially with a hermetic setup.

Please check the [playground](./playground.md) I prepared to become more familiar with them.

As you can see, with just a few commands, you’re already equipped to do most things with Bazel. There are more advanced commands out there, but they are often for very specific use cases, so no need to worry about them for now.

## Final Thoughts

Because Bazel supports multiple programming languages and provides a human-readable abstraction layer on top of them, it's really straightforward to create, build, run, and test multiple targets.

Bazel also comes with powerful features out of the box. One great example is its local caching mechanism. While there are ways to improve it and even turn it into a remote cache, the default mechanism is already great.

We won't dive into the details of Bazel's caching just yet, but one key way to improve cache hits is by making our builds _hermetic_. This not only boosts cache effectiveness but also makes our builds more consistent across different machines—say goodbye to "it works on my machine."

Stay tuned for the next articles, where we’ll use Bazel’s built-in dependency manager to make our builds more hermetic and reduce reliance on system dependencies.
