# Security Policy

## Supported Versions

Axiom is in early development and has no stable releases yet. Security fixes land on the `main` branch only.

| Version | Supported |
|---|---|
| Latest `main` | ✅ |
| Anything older | ❌ |

## Reporting a Vulnerability

If you discover a security vulnerability in this repository, please report it privately with [GitHub's built-in reporting tool](https://github.com/mrsumanbiswas/project-axiom/security/advisories/new). Please do not open a public issue. We appreciate responsible disclosure and will acknowledge your report as soon as possible.

A useful report includes:

- the commit you tested (`git rev-parse --short HEAD`),
- the input or steps that trigger the problem,
- your OS, compiler and LLVM versions,
- the impact as you understand it.

## Scope

Reports are especially welcome for:

- Memory-safety bugs that Axiom source text can trigger, such as crashes, out-of-bounds reads or use-after-free in the lexer (`src/lexer/`) or parser (`src/parser/`), especially on malformed input
- Code generation bugs that produce invalid or miscompiled LLVM IR (`src/codegen/`)

## Security Model

Axiom compiles programs to native code. **It is not a sandbox.** Once JIT execution lands, an Axiom program will run as native machine code with the full privileges of the `axiom` process. Its `extern` declarations will be able to call any function the host process exports, including the entire C library. Running an untrusted Axiom program is therefore equivalent to running an untrusted native executable.

Because of this, the following are **not** vulnerabilities:

- a program that uses `extern` to call host functions, including to do something harmful,
- a program that you choose to run exhausting CPU time or memory.

Thank you for helping to keep Axiom secure!
