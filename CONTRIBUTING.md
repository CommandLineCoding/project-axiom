# Contributing to Axiom

Thanks for your interest in Axiom! This guide explains how to set up a development build, which conventions the codebase follows and how changes get merged.

Everyone taking part is expected to follow the [Code of Conduct](CODE_OF_CONDUCT.md).

## Ways to Contribute

- **Report bugs** using the bug report template, and include the smallest input that reproduces the problem.
- **Propose features** using the feature request template. Language features especially benefit from discussion before anyone writes code.
- **Pick up a roadmap item** from the [README](README.md#roadmap). Comment on the matching issue, or open one, before you start so that work isn't duplicated.
- **Improve the docs.** If something in [`docs/`](docs/) was unclear or wrong, a fix is very welcome.

To report a security vulnerability, follow [SECURITY.md](SECURITY.md) instead of opening a public issue.

## Development Setup

1. Install the [prerequisites](README.md#prerequisites): CMake 3.20+, GCC 14+ or Clang 18+, and the LLVM development package.

2. Fork the repository and clone your fork:

   ```bash
   git clone https://github.com/<your-username>/project-axiom.git
   cd project-axiom
   ```

3. Configure a debug build. The second flag exports compile commands for clangd and other IDE tooling:

   ```bash
   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
   cmake --build build
   ./build/axiom
   ```

   If CMake can't find LLVM, see [Build](README.md#build) in the README.

4. Before you open a pull request, also build with the other compiler. The `build-*/` directories are git-ignored:

   ```bash
   CC=clang CXX=clang++ cmake -S . -B build-clang -G Ninja
   cmake --build build-clang
   ```

New to LLVM? LLVM's [Kaleidoscope tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html) walks through the same pipeline that Axiom is built on. After that, read [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for how Axiom implements it and [docs/LANGUAGE.md](docs/LANGUAGE.md) for the language it accepts.

## Coding Conventions

Match the style of the surrounding code. The project has no `.clang-format` yet, so these rules are the reference.

### Language

- Use C++23 with no compiler extensions. Prefer standard facilities such as `std::expected`, `std::print` / `std::println`, `std::format` and `std::string_view`.
- **Don't use exceptions for errors in user input.** Return `Expected<T>`, and create errors with `std::unexpected(CompilerError{ .message = ..., .location = ... })`, using the location of the offending token.
- Use `std::unique_ptr` for owned objects. Raw pointers are only for non-owning references, such as the `llvm::Value*` values that LLVM's API returns.
- Mark accessors and other queries `[[nodiscard]]`, and mark functions that cannot fail `noexcept`.
- Initialize aggregates with designated initializers: `Token{ .kind = ..., .lexeme = ..., .location = ... }`.
- Keep LLVM headers out of the frontend headers (`token.h`, `lexer.h`, `ast.h`, `parser.h`). Forward-declare the LLVM types you need instead.

### Naming

| Kind | Style | Examples |
|---|---|---|
| Classes, structs and enum classes | `PascalCase` | `CodeGenerator`, `TokenKind` |
| Enumerators | `PascalCase` | `TokenKind::Identifier` |
| Functions, methods and variables | `snake_case` | `next_token()`, `parse_bin_op_rhs()`, `fn_name` |
| Private data members | `m_` + `snake_case` | `m_cursor`, `m_current_tok` |
| Namespaces | lowercase | `axiom` |
| Files | `snake_case` | `source_location.h`, `codegen.cpp` |

### Formatting

- Indent with 4 spaces, never tabs.
- Start every header with `#pragma once`.
- Include project headers by their full path in quotes (`#include "axiom/support/error.h"`), and standard and LLVM headers in angle brackets.
- Put public headers in `include/axiom/` and implementations in `src/<stage>/`.
- Close namespaces with a comment: `} // namespace axiom`.

### Warnings

Every target is compiled with `set_project_warnings()` from [`cmake/CompilerWarnings.cmake`](cmake/CompilerWarnings.cmake), which enables `-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wold-style-cast` and more. **New code must not add warnings under GCC or Clang.** Fix the cause instead of silencing it. For example, use `static_cast` rather than a C-style cast.

### Adding a new compiler stage

1. Add a public header at `include/axiom/<stage>.h`.
2. Create `src/<stage>/CMakeLists.txt` from the same template as the existing libraries:

   ```cmake
   ADD_LIBRARY(axiom_<stage> STATIC)

   TARGET_SOURCES(axiom_<stage> PRIVATE
       <stage>.cpp
   )

   TARGET_INCLUDE_DIRECTORIES(axiom_<stage> PUBLIC
       ${CMAKE_SOURCE_DIR}/include
   )

   TARGET_COMPILE_FEATURES(axiom_<stage> PUBLIC
       cxx_std_23
   )

   SET_PROJECT_WARNINGS(axiom_<stage>)
   ```

3. Add `ADD_SUBDIRECTORY(src/<stage>)` to the root `CMakeLists.txt`. Then add the library to `TARGET_LINK_LIBRARIES(axiom ...)`, before every library it depends on. See [Build Structure](docs/ARCHITECTURE.md#3-build-structure) for why the order matters.
4. Describe the new stage in [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Testing

Axiom has no automated test suite yet. The `BUILD_TESTING` option is reserved for it, and setting it up is on the roadmap. Proposals are welcome. Until it exists, do the following before you open a pull request:

- Build cleanly with both GCC and Clang.
- Run `./build/axiom`.
- For lexer, parser or codegen changes, try your change on sample input, including malformed input, and describe what you tried in the pull request.

## Git Workflow

### Branches

Branch from `main` and name the branch `<issue-number>-<short-description>`, for example `1-documentation`. This is the name GitHub suggests when you create a branch from an issue.

### Commit messages

Use [Conventional Commits](https://www.conventionalcommits.org/):

```text
<type>(<scope>): <summary>
```

- **Types:** `feat`, `fix`, `docs`, `refactor`, `test`, `build`, `ci`, `perf`, `chore`.
- **Scope:** the stage you changed, such as `lexer`, `parser`, `ast`, `codegen`, `support`, or later `jit` and `repl`. Leave the scope out for repository-wide changes.
- **Summary:** write it in the imperative mood and lowercase, with no trailing period.
- Make one logical change per commit.

These examples come from the project's history:

```text
feat(lexer): tokenize numeric literals
feat(parser): implement binary operator precedence parsing
build: wire up parser directory in CMake and implement Parser skeleton
```

### Pull requests

1. Keep each pull request focused on one change. Small pull requests get reviewed faster.
2. Fill in the [pull request template](.github/pull_request_template.md), and link the issue it closes.
3. Update the docs when behaviour changes:
   - [docs/LANGUAGE.md](docs/LANGUAGE.md) for syntax, semantics or diagnostics,
   - [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the pipeline or the build,
   - the [README roadmap](README.md#roadmap) when an item lands.
4. Address review feedback by pushing new commits to the same branch.

## License

By contributing, you agree that your contributions will be licensed under the [MIT License](LICENSE), the same license that covers the project.
