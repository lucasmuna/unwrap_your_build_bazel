# Hermetic Toolchains and Libraries

By now, you should have a good grasp of what a Bazel workspace is, how to create packages and targets, and how to use the most common commands. You’ve also seen how its out-of-the-box caching mechanism works.

Our next goal is to improve reproducibility, reliability, and cache hits, which also means faster incremental builds. To achieve this, we'll work on ensuring our Bazel targets have **hermetic builds**.

## Playground

If you want to play with some examples directly, check out [playground.md](./playground.md).

## Hermetic Build

According to the [official Bazel documentation](https://bazel.build/basics/hermeticity):

> "When given the same input source code and product configuration, a hermetic build system always returns the same output by isolating the build from changes to the host system.”

In simpler terms, a hermetic build ensures that your code builds the same way every time, regardless of the machine it's on. This is crucial for consistency and reliability.

While Bazel provides the tools for self-contained builds, it's up to us to set them up correctly. To do this, we first need to identify what our code depends on from the host machine. Once we know that, we can start providing these dependencies hermetically, making our builds as insensitive as possible to the software installed on the host.

### Toolchains

Having a hermetic toolchain is one of the most important steps toward achieving a reproducible and reliable build. A toolchain is essentially the set of tools used to build your targets, like compilers and linkers.

While you can create your own Bazel toolchain, I suggest starting with what's already available. Luckily, there are open-source solutions for nearly every programming language and even some frameworks. For example:

- **C/C++** - `toolchains_llvm` - [https://github.com/bazel-contrib/toolchains_llvm](https://github.com/bazel-contrib/toolchains_llvm)
- **Python** - `rules_python` - [https://github.com/bazel-contrib/rules_python](https://github.com/bazel-contrib/rules_python)
- **Go** - `rules_go` - [https://github.com/bazel-contrib/rules_go](https://github.com/bazel-contrib/rules_go)
- **Rust** - `rules_rust` - [https://github.com/bazelbuild/rules_rust](https://github.com/bazelbuild/rules_rust)
- **TypeScript** - `rules_ts` - [https://github.com/aspect-build/rules_ts](https://github.com/aspect-build/rules_ts)
- **Node.js** - `rules_nodejs` - [https://github.com/bazel-contrib/rules_nodejs](https://github.com/bazel-contrib/rules_nodejs)

### Libraries

Once a proper hermetic toolchain is set up, any external libraries your target depends on must also be injected into it. For example, if you have a Python test that uses `pytest`, you'll need to set it up and add it as a dependency since it's not a built-in Python module.

For some languages, configuring hermetic external libraries is easy. For others, it might depend on whether the library already offers a Bazel integration. This often depends on whether the language has a standard package manager. For example:

- **Python**: It has two main package managers, `pip` and `uv`. Both are integrated into Bazel via `rules_python`, so you can expect a familiar interface.
- **C/C++**: There's no standard package manager. While some libraries like `googletest` offer a Bazel integration, others might not. In those cases, you'll have to figure out how to integrate them. Sometimes, simply packing the library into a tarball is enough; other times, it can be more tricky. There are solutions like [`rules_foreign_cc`](https://github.com/bazel-contrib/rules_foreign_cc) that make it easier to integrate CMake-based libraries into Bazel.

### Sandbox

With hermetic toolchains and libraries in place, it's mostly up to Bazel to ensure that your code can't access your host environment for libraries, variables, or other dependencies. To achieve this, Bazel creates a [sandbox](https://bazel.build/docs/sandboxing) for each action. This sandbox contains symlinks to all specified dependencies and the configured toolchain, meaning that actions (like builds or tests) will only have access to what was strictly specified.

Of course, this doesn't mean Bazel's sandbox is bulletproof. It's not that hard for a user to break out of the sandbox—Bazel even has options to allow it—but then it will be explicit that an action is accessing something from the host system.

Bazel's sandbox can be a bit costly, as it needs to create a lot of symlinks (or, on Windows, hard copies), but they are essential for guaranteeing hermetic builds. Once you get used to it, you won't want to go back to non-hermetic actions.

### Common Pitfalls

Non-hermetic builds can happen for mysterious reasons. The most important thing is not to fool yourself. If you can't make everything hermetic at once, tag the non-hermetic actions and solve them later—it happens. Bazel even has some [special tags](https://bazel.build/reference/be/common-definitions#common.tags) for this.

Here are some common pitfalls I can remember:

- Accessing hard-coded absolute paths or somehow finding your own way to break the sandbox.
- Relying on the network or on timestamps.
- Running subprocesses that might not be hermetic (e.g., they might rely on the current user's home directory).
- Relying on host system features that are usually taken for granted, like `bash`, `shell`, `PowerShell`, `zip`, etc.
- Setting up hermetic toolchains and libraries but not configuring Bazel to use them.
- And many others.

Bazel also documents the basics of how to [identify](https://bazel.build/basics/hermeticity#nonhermeticity) and [troubleshoot](https://bazel.build/basics/hermeticity#troubleshooting-nonhermeticity) non-hermetic builds.

## Examples

Finally, some code! Sorry for all that text. Let's jump right into it. We're going to start from where we left off in the [previous article](../1_workspace_commands_and_targets/README.md).

### C++ Hermetic Build Example

Let's start by setting up a hermetic `LLVM` toolchain and making the `googletest` library available. Add the following to your `MODULE.bazel` file:

```python
# MODULE.bazel
# Existing code ...

bazel_dep(name = "toolchains_llvm", version = "1.4.0")

llvm = use_extension("@toolchains_llvm//toolchain/extensions:llvm.bzl", "llvm")
llvm.toolchain(
    llvm_version = "16.0.0",
)
use_repo(llvm, "llvm_toolchain")

register_toolchains("@llvm_toolchain//:all")

bazel_dep(name = "googletest", version = "1.17.0")
```

With that, we now have a hermetic `LLVM` toolchain. You can use it by simply running a command like `bazel build //cc_example/...` to build those targets. Note that Bazel will first download the toolchain and then use it to build the targets.

The next step is to use `googletest`. For that, we must modify both the `cc_example/BUILD` and `cc_example/test.cpp` files. Replace the `//cc_example:test` target definition with:

```python
# cc_example/BUILD
# Existing code ...

cc_test(
    name = "test",
    srcs = ["test.cpp"],
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
```

And replace the content of the `cc_example/test.cpp` file with:

```cpp
// cc_example/test.cpp

#include "lib.h"

#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions)
{
    printMessage("Unwrap Your Build");
    EXPECT_EQ(7 * 6, 42);
}
```

At this point, you can build and test the C++ targets. Bazel will fetch `googletest` and inject it into the `//cc_example:test` target. If you try to comment out the `googletest` dependencies, the build will fail.

### Python Hermetic Build Example

Let's start by setting up a hermetic Python toolchain and its package manager, `pip`, with `pytest` as the only library. Add the following to your `MODULE.bazel` file:

```python
# MODULE.bazel
# Existing code ...

bazel_dep(name = "rules_python", version = "1.5.3")

python = use_extension("@rules_python//python/extensions:python.bzl", "python")
python.toolchain(
    python_version = "3.13",
)

pip = use_extension("@rules_python//python/extensions:pip.bzl", "pip")
pip.parse(
    hub_name = "our_pip_hub",
    python_version = "3.13",
    requirements_lock = "//:requirements_lock.txt",
)
use_repo(pip, "our_pip_hub")
```

The next step is to create our requirements files. There are two: `requirements.in` and `requirements_lock.txt`. The first is our input, and the second is the `pip`-resolved version of it. Create an empty `requirements_lock.txt` file and a `requirements.in` file with the following content:

```python
# requirements.in

pytest==8.4.1
```

Now, add the following code to your root `BUILD` file:

```python
# BUILD

load("@rules_python//python:pip.bzl", "compile_pip_requirements")

# Available executable targets:
# - //:py_requirements.update
# - //:py_requirements.test
compile_pip_requirements(
    name = "py_requirements",
    src = "requirements.in",
    requirements_txt = "requirements_lock.txt",
)
```

At this point, you can run `bazel run //:py_requirements.update`. This will resolve and lock the requirements from `requirements.in` into `requirements_lock.txt`.

With `requirements_lock.txt` populated, `pytest` is finally available. But before using it, we need to modify the Python test target and file. Modify `py_example/BUILD` with:

```python
# py_example/BUILD

load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")

# Existing code ...

py_test(
    name = "test",
    srcs = ["test.py"],
    deps = [
        ":example",  # It implicitly depends on `:lib` as well.
        "@our_pip_hub//pytest:pkg",
    ],
)
```

And replace the content of `py_example/test.py` with:

```python
import sys

import pytest

from py_example import example


@pytest.mark.parametrize("expected", [None])
def test(expected):
    assert example.main() == expected


if __name__ == "__main__":
    sys.exit(pytest.main([__file__]))
```

If you now run `bazel test //py_example/...`, Bazel will first download the `rules_python` repo, the Python toolchain with the selected interpreter version, and the locked `pytest` version using `pip`. Then, it will run all tests under the `//py_example` package.

## Commands

I recommend you play around with the current setup to get more familiar with it. Some interesting ideas are to try adding more libraries or removing the dependencies we added above. I've written some ideas in the [playground](./playground.md).

## Final Thoughts

As I mentioned, Bazel only offers solutions to achieve hermetic builds; it's up to us to configure them properly. To be honest, these setups can get as complex as you want, but the basic infrastructure, which already provides a lot of benefits, is relatively easy to achieve.

My main advice is to go step by step and be honest with your code and yourself. Don't try to mask something that isn't hermetic as hermetic. Instead, mark it with the proper Bazel tag and make the action non-hermetic so you can improve it in the future. It's always a trade-off between how hermetic (i.e., how insensitive to the host machine) you want your targets to be and how much time you have to spend on the setup.

I'm still not sure what the next article will be about, probably about Bzlmod and Bazel Central Registry. Let me know if you have any other ideas.
