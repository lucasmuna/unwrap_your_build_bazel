load("@rules_python//python:pip.bzl", "compile_pip_requirements")

# Available executable targets:
# - //:py_requirements.update
# - //:py_requirements.test
compile_pip_requirements(
    name = "py_requirements",
    src = "requirements.in",
    requirements_txt = "requirements_lock.txt",
)
