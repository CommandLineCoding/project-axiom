[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=flat-square)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat-square&logo=cplusplus)](https://en.cppreference.com/w/cpp/23)
[![LLVM](https://img.shields.io/badge/LLVM-22-262D3A?style=flat-square&logo=llvm)](https://llvm.org/)
[![Status: early development](https://img.shields.io/badge/status-early%20development-orange?style=flat-square)](#roadmap)

---

# Axiom

Axiom is a small, expression-oriented programming language with a compiler written from scratch in modern **C++23** on top of **LLVM**. The goal is an interactive, Just-In-Time (JIT) compiled language. You type code into a REPL, Axiom compiles it to native machine code in memory with LLVM's ORC JIT, and it runs immediately.

> **Status: early development (v0.1.0).** The lexer, the parser and LLVM IR generation for expressions work today. The next milestones are code generation for functions, the JIT engine and the interactive REPL. See the [Roadmap](#roadmap). Until the REPL lands, the `axiom` binary only runs a smoke test of the code generator.

## A Taste of Axiom

```text
# Comments run from '#' to the end of the line.
extern sin(x)              # declare a function provided by the host (e.g. libm)

def square(x) x * x        # parameters are separated by spaces

square(4) + sin(0)         # arguments are separated by commas
```

Every value is a 64-bit floating-point number. Function bodies are single expressions, and their value is the result. The binary operators are `<`, `+`, `-` and `*`. The parser accepts the program above today, but running it requires the JIT, which is still in development.

The full syntax, operator table and error messages are in the [Language Reference](docs/LANGUAGE.md).

## Features

These features work today:

- **Strict C++23:** errors use `std::expected`, output uses `<print>`, and tokens print through a custom `std::formatter`. Compiler extensions are disabled (`CMAKE_CXX_EXTENSIONS OFF`).
- **Zero-copy lexer:** tokens are non-owning `std::string_view` slices of the source buffer, each tagged with a 1-based line and column. The lexer makes no heap allocations.
- **Exception-free diagnostics:** parsing functions return `Expected<T>`, an alias for `std::expected<T, CompilerError>`. An error carries a message and a source location, and prints as `axiom:LINE:COL: error: MESSAGE`.
- **Operator-precedence climbing:** a precedence table drives the parsing of binary expressions. Operators are left-associative, and the grammar has no left recursion.
- **Single-token lookahead:** the parser chooses each rule from the kind of the current token and never backtracks.
- **LLVM IR generation for expressions:** numbers, variables, binary operators and calls lower to IR through `llvm::IRBuilder`. Constant expressions are folded as the IR is built, so `10 + 5 * 2` becomes the constant `20.0`.
- **Modular build:** each compiler stage is its own static library. Every target compiles under a strict warning set (`-Wall -Wextra -Wpedantic -Wshadow -Wconversion ...`).

## Tech Stack

| Layer | Technology |
|---|---|
| Language | C++23 (GCC or Clang, no compiler extensions) |
| Compiler backend | LLVM 22: Core, Analysis, ExecutionEngine, OrcJIT, Support and the native target |
| Error handling | `std::expected` (no exceptions) |
| Build | CMake 3.20+ with Ninja or Make |

---

## Getting Started

### Prerequisites

| Requirement | Version | Notes |
|---|---|---|
| CMake | 3.20 or newer | |
| C++ compiler | GCC 14+ or Clang 18+ | The standard library must provide C++23 `<print>` and `<expected>` |
| LLVM | Development package with CMake config files | Developed against LLVM 22 |
| Ninja | Any | Optional, for faster builds |

The build has been tested on Fedora 44 with GCC 16.2, Clang 22.1 and LLVM 22.1. Other platforms should work, but they haven't been tested yet. Reports and fixes are welcome.

```bash
# Fedora (tested)
sudo dnf install gcc-c++ cmake ninja-build llvm-devel

# Ubuntu 24.04 / Debian (not yet tested)
sudo apt install g++-14 cmake ninja-build llvm-18-dev

# macOS with Homebrew (not yet tested)
brew install llvm cmake ninja
```

### Build

```bash
git clone https://github.com/mrsumanbiswas/project-axiom.git
cd project-axiom

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Drop `-G Ninja` to use CMake's default generator (usually Make).

Outside Fedora, CMake may fail to find LLVM or its headers. If so, point it at your LLVM installation. Use `llvm-config-18` on Ubuntu and `$(brew --prefix llvm)/bin/llvm-config` on macOS:

```bash
cmake -S . -B build -G Ninja \
  -DLLVM_DIR="$(llvm-config --cmakedir)" \
  -DCMAKE_CXX_FLAGS="-I$(llvm-config --includedir)"
```

The include flag works around a known gap: the `axiom_codegen` library does not yet add LLVM's include directory itself.

### Run

```bash
./build/axiom
```

```text
>_ Testing Expression & Operator Codegen
Success! Expression compiled cleanly without validation faults.
```

For now, the binary builds the syntax tree for `10 + 5 * 2` by hand and lowers it to LLVM IR. It will become the REPL once the driver lands.

### Build Options

| Option | Default | Description |
|---|---|---|
| `CMAKE_BUILD_TYPE` | *(empty)* | `Debug`, `Release`, `RelWithDebInfo` or `MinSizeRel` |
| `LLVM_DIR` | auto-detected | Directory containing `LLVMConfig.cmake` (`llvm-config --cmakedir`) |
| `CMAKE_EXPORT_COMPILE_COMMANDS` | `OFF` | Writes `build/compile_commands.json` for clangd and other tooling |
| `BUILD_TESTING` | `ON` | Reserved for the upcoming test suite; has no effect yet |

### Troubleshooting

| Symptom | Fix |
|---|---|
| `Could not find a package configuration file provided by "LLVM"` | Install the LLVM development package and pass `-DLLVM_DIR="$(llvm-config --cmakedir)"` |
| `fatal error: llvm/IR/LLVMContext.h: No such file or directory` | LLVM's headers are not on the default include path. Pass `-DCMAKE_CXX_FLAGS="-I$(llvm-config --includedir)"` |
| `fatal error: print: No such file or directory` or `'print' file not found` | Your standard library predates C++23 `<print>`. Use GCC 14+ or Clang 18+ |

---

## Architecture

```text
          Source text / REPL input
                     │
                     ▼
         ┌───────────────────────┐
         │         Lexer         │  ◄── zero-copy std::string_view tokens
         └───────────────────────┘
                     │  Token
                     ▼
         ┌───────────────────────┐
         │        Parser         │  ◄── operator-precedence climbing
         └───────────────────────┘
                     │  AST: NumExpr, VarExpr, BinaryExpr, CallExpr, FuncNode
                     ▼
         ┌───────────────────────┐
         │     CodeGenerator     │  ◄── LLVMContext, IRBuilder, Module
         └───────────────────────┘
                     │  LLVM IR
                     ▼
         ┌───────────────────────┐
         │    JIT  (planned)     │  ◄── LLVM ORC JIT (LLJIT)
         └───────────────────────┘
```

| Stage | Library | Header | Status |
|---|---|---|---|
| Support | `axiom_support` | [`include/axiom/support/`](include/axiom/support/) | Done |
| Lexer | `axiom_lexer` | [`lexer.h`](include/axiom/lexer.h), [`token.h`](include/axiom/token.h) | Done |
| Parser | `axiom_parser` | [`parser.h`](include/axiom/parser.h), [`ast.h`](include/axiom/ast.h) | Done |
| Code generator | `axiom_codegen` | [`codegen.h`](include/axiom/codegen.h) | Expressions done; functions in progress |
| JIT and REPL | | | Planned |

The [Architecture guide](docs/ARCHITECTURE.md) traces an expression through every stage. It also covers the error model, ownership rules and build structure.

## Project Structure

```
project-axiom/
├── .github/                    # Issue and pull request templates
├── cmake/
│   └── CompilerWarnings.cmake  # set_project_warnings(): strict warning flags per target
├── docs/
│   ├── ARCHITECTURE.md         # Compiler pipeline, design decisions, build structure
│   └── LANGUAGE.md             # Language reference: syntax, operators, diagnostics
├── include/axiom/              # Public headers
│   ├── support/
│   │   ├── error.h             # CompilerError, Expected<T>, report_error()
│   │   └── source_location.h   # SourceLocation (line, column)
│   ├── token.h                 # TokenKind, Token, std::formatter<Token>
│   ├── lexer.h                 # Lexer
│   ├── ast.h                   # AST node classes
│   ├── parser.h                # Parser
│   └── codegen.h               # CodeGenerator (LLVM context, IR builder, module)
├── src/
│   ├── support/                # axiom_support library
│   ├── lexer/                  # axiom_lexer library
│   ├── parser/                 # axiom_parser library
│   ├── codegen/                # axiom_codegen library (AST → LLVM IR)
│   └── main.cpp                # axiom executable (code generator smoke test for now)
├── CMakeLists.txt
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── SECURITY.md
└── LICENSE
```

## Roadmap

- [x] Lexer: identifiers, keywords (`def`, `extern`), numbers, comments and source locations
- [x] Parser: expressions, calls, operator precedence, plus `def`, `extern` and top-level expressions
- [x] IR generation for numbers, variables, binary operators and calls
- [ ] IR generation for prototypes and function definitions
- [ ] Native execution with LLVM ORC JIT (`llvm::orc::LLJIT`)
- [ ] `extern` functions resolved against host-process symbols (`sin`, `cos`, `sqrt`, ...)
- [ ] Interactive REPL, in which each top-level expression (`__anon_expr`) is compiled under its own `llvm::orc::ResourceTracker` and freed after it runs
- [ ] `if` / `then` / `else` expressions, lowered to basic blocks joined by an SSA PHI node
- [ ] Unit tests (the `BUILD_TESTING` option is already reserved)

## Contributing

Contributions are welcome. [CONTRIBUTING.md](CONTRIBUTING.md) covers the development setup, coding conventions and commit message format. For larger changes, please open an issue with one of the [issue templates](.github/ISSUE_TEMPLATE/) first. When you open a pull request, fill in the [pull request template](.github/pull_request_template.md). Please also read the [Code of Conduct](CODE_OF_CONDUCT.md).

To report a security issue, follow [SECURITY.md](SECURITY.md) instead of opening a public issue.

## Acknowledgements

Axiom's syntax and code generation build on LLVM's [Kaleidoscope tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html). It is the best background reading for anyone new to the codebase.

## License

Axiom is licensed under the MIT License. See [LICENSE](LICENSE) for details.
