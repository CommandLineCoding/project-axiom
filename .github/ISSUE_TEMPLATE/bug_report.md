---
name: Bug Report
about: Report a crash, a wrong result or a build failure
title: '[BUG]: '
labels: bug
assignees: ''
---

## Description
A clear and concise description of what the bug is.

## To Reproduce
The smallest Axiom input or code change that triggers the bug:

```text
# Axiom source here
```

The commands you ran:

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/axiom
```

## Expected Behavior
What should happen?

## Actual Behavior
What happened instead? Paste the full output, including any `axiom:LINE:COL: error: ...` diagnostics or compiler errors.

## Environment
- Axiom commit: [output of `git rev-parse --short HEAD`]
- OS: [e.g. Fedora 44]
- Compiler: [e.g. GCC 16.2 or Clang 22.1]
- LLVM: [output of `llvm-config --version`]
- CMake: [output of `cmake --version`]
