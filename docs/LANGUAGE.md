# Axiom Language Reference

This reference describes the language as implemented in v0.1.0. The lexer and parser accept everything described here. Code generation currently covers expressions only, and running programs requires the JIT, which is still on the [roadmap](../README.md#roadmap). Behaviour that is designed but not yet built is marked **Planned**.

## Contents

1. [Overview](#1-overview)
2. [Lexical Structure](#2-lexical-structure)
3. [Grammar](#3-grammar)
4. [Expressions](#4-expressions)
5. [Functions](#5-functions)
6. [Top-Level Expressions](#6-top-level-expressions)
7. [Diagnostics](#7-diagnostics)
8. [Limitations and Gotchas](#8-limitations-and-gotchas)

---

## 1. Overview

Axiom is expression-oriented. A function body is a single expression, and the value of that expression is the function's result.

There is exactly one data type: the 64-bit IEEE 754 floating-point number (`double`). Comparisons produce `1.0` for true and `0.0` for false.

A program is a sequence of top-level items. Each item is one of the following:

| Item | Example | Meaning |
|---|---|---|
| Function definition | `def square(x) x * x` | Defines a function |
| External declaration | `extern sin(x)` | Declares a function that is implemented elsewhere |
| Top-level expression | `square(4) + 1` | An expression to evaluate |

```text
extern sin(x)
def square(x) x * x
square(4) + sin(0)
```

---

## 2. Lexical Structure

### Source text

Source is read as a sequence of bytes and treated as ASCII. Every position has a 1-based `line:column` location. Each character, including a tab, advances the column by one, and a newline starts a new line.

### Whitespace and comments

Spaces, tabs, carriage returns and newlines separate tokens and are otherwise ignored. A `#` starts a comment that runs to the end of the line.

```text
def square(x) x * x   # this is a comment
```

### Tokens

| Kind | Printed as | Rule | Examples |
|---|---|---|---|
| Keyword | `DEF`, `EXTERN` | The exact words `def` and `extern` | `def`, `extern` |
| Identifier | `IDENTIFIER` | `[A-Za-z_][A-Za-z0-9_]*` that is not a keyword | `x`, `_tmp`, `x_1` |
| Number | `NUMBER` | `[0-9]+(\.[0-9]*)?` or `\.[0-9]+` | `42`, `3.14`, `.5`, `5.` |
| Operator | `OPERATOR` | Any other single character | `+`, `<`, `(`, `)`, `,` |
| End of input | `EOF` | No characters left | |

- Every operator token is exactly one character long. `<=` lexes as two tokens, `<` and `=`.
- Parentheses and commas are operator tokens too. The parser recognises them by their text.
- A number contains at most one decimal point. `1.2.3` lexes as the two numbers `1.2` and `.3`.
- There is no exponent notation. `1e5` lexes as the number `1` followed by the identifier `e5`.
- Keywords are case-sensitive, so `Def` is an identifier.

Tokens print through `std::format` as `KIND ('lexeme') at LINE:COLUMN`. For example, the line `def add(a b) a + b` lexes as:

```text
DEF ('def') at 1:1
IDENTIFIER ('add') at 1:5
OPERATOR ('(') at 1:8
IDENTIFIER ('a') at 1:9
IDENTIFIER ('b') at 1:11
OPERATOR (')') at 1:12
IDENTIFIER ('a') at 1:14
OPERATOR ('+') at 1:16
IDENTIFIER ('b') at 1:18
EOF ('') at 1:19
```

---

## 3. Grammar

The grammar below uses EBNF. Terminals are quoted, and `identifier` and `number` are the tokens defined above.

```ebnf
program     = { top_level } ;
top_level   = definition | external | expression ;

definition  = "def" , prototype , expression ;
external    = "extern" , prototype ;
prototype   = identifier , "(" , { identifier } , ")" ;

expression  = primary , { binary_op , primary } ;
primary     = number
            | identifier
            | identifier , "(" , [ expression , { "," , expression } ] , ")"
            | "(" , expression , ")" ;
binary_op   = "<" | "+" | "-" | "*" ;
```

- The grammar does not say how operands group. [Operator precedence](#binary-operators) decides that.
- Prototype parameters are separated by whitespace, while call arguments are separated by commas.
- An identifier followed directly by `(` is a call. Otherwise it is a variable reference.
- The parser has one entry point per `top_level` alternative: `parse_definition()`, `parse_extern()` and `parse_top_level_expression()`. The caller picks one by looking at the current token: `def`, `extern`, or anything else. The upcoming REPL driver will run that loop.

---

## 4. Expressions

### Numbers

A number literal evaluates to its `double` value. `.5` is `0.5` and `5.` is `5.0`.

### Variables

The only variables are the parameters of the enclosing function. Referencing any other name is a code generation error. There is no assignment and there are no local variables.

### Binary operators

| Operator | Precedence | Associativity | Result | LLVM lowering |
|---|---|---|---|---|
| `*` | 40 | Left | Product | `fmul` |
| `+` | 20 | Left | Sum | `fadd` |
| `-` | 20 | Left | Difference | `fsub` |
| `<` | 10 | Left | `1.0` if the left side is less than the right, otherwise `0.0` | `fcmp ult`, then `uitofp` to `double` |

A higher precedence binds more tightly. Parentheses override precedence.

`<` uses LLVM's *unordered* less-than comparison (`ult`), which is true when either operand is NaN. A comparison involving NaN therefore evaluates to `1.0`.

| Expression | Groups as | Value |
|---|---|---|
| `1 + 2 * 3` | `1 + (2 * 3)` | `7` |
| `(1 + 2) * 3` | `(1 + 2) * 3` | `9` |
| `10 - 4 - 3` | `(10 - 4) - 3` | `3` |
| `1 + 2 < 4` | `(1 + 2) < 4` | `1` |
| `2 < 1` | `2 < 1` | `0` |
| `.5 + 5.` | `0.5 + 5.0` | `5.5` |

The values above come from Axiom's code generator, which folds constant expressions while it builds IR.

### Calls

```text
name(arg1, arg2, ...)
```

Each argument is a full expression, and `f()` is a call with no arguments. During code generation, the callee is looked up in the current LLVM module and the argument count must match its parameter count.

---

## 5. Functions

### Definitions

```text
def name(param1 param2 ...) body
```

- Separate parameters with whitespace. `def add(a, b) a + b` is a syntax error.
- A function can have no parameters: `def answer() 42`.
- The body is a single expression, and its value is the function's result.
- The body ends at the first token that cannot continue the expression. Because of this, items need no separator: `def add(a b) a + b add(1, 2)` is a definition followed by a top-level expression.

All parameters and the return value are `double`.

> **Planned.** Code generation for function definitions is not implemented yet. `FuncNode::codegen()` returns `nullptr`.

### External declarations

```text
extern name(param1 param2 ...)
```

An external declaration gives a function's signature without a body, so that it can be called.

> **Planned.** The JIT will resolve external declarations against symbols in the host process. This means C library math functions such as `sin`, `cos` and `sqrt` can be called directly once they are declared. `Prototype::codegen()` currently returns `nullptr`.

---

## 6. Top-Level Expressions

A top-level expression is any expression that is not part of a `def` or an `extern`. The parser wraps it in an anonymous function named `__anon_expr` that has no parameters. It then goes through the same pipeline as any other function definition.

> **Planned.** The REPL will JIT-compile `__anon_expr`, call it and print the result. Each expression is added to the JIT under its own `llvm::orc::ResourceTracker`, so its machine code is freed after it runs and the next expression can reuse the name.

---

## 7. Diagnostics

Parser errors print to standard error in this format:

```text
axiom:LINE:COLUMN: error: MESSAGE
```

The location points at the token that caused the error. When input ends early, it points just past the last character. The parser stops at the first error and does not attempt recovery.

| Message | Example input | Reported at |
|---|---|---|
| `Unknown token type encountered when expecting an expression node` | `-5` | `1:1` |
| `Unknown token type encountered when expecting an expression node` | `1 +` | `1:4` |
| `Expected matching closing parenthesis ')'` | `(1 + 2` | `1:7` |
| `Expected ',' or ')' inside parameter collection` | `foo(1 2)` | `1:7` |
| `Expected function name in prototype signature` | `def (x) x` | `1:5` |
| `Expected '(' after function name in signature` | `def f x` | `1:7` |
| `Expected closing ')' after prototype parameters` | `def add(a, b) a + b` | `1:10` |
| `Failed to parse numeric literal values` | A literal too large for a `double`, such as `1` followed by 400 zeros | `1:1` |

Code generation errors currently print as `Error: ...` without a source location, and the failing node yields no IR:

| Message | Cause |
|---|---|
| `Error: Unknown variable reference name: NAME` | `NAME` is not a parameter in scope |
| `Error: Unknown function referenced: NAME` | No function named `NAME` exists in the module |
| `Error: Incorrect number of arguments passed to function` | The call's argument count differs from the function's parameter count |
| `Error: Invalid binary operator option: OP` | The operator has no lowering. The parser never produces this case |

---

## 8. Limitations and Gotchas

- **No unary operators.** `-5` is a syntax error. Write `0 - 5` instead.
- **Unsupported operators end the expression instead of failing.** Only `<`, `+`, `-` and `*` are binary operators. Any other operator character, such as `/`, simply ends the expression. For example, `6 / 2` parses as `6`, and `/ 2` is left over for the next top-level item, which then fails to parse.
- **No statement separator.** `;` is not part of the grammar. For example, `square(4);` parses `square(4)` and then fails on `;`.
- **No control flow, assignment or local variables yet.** `if` / `then` / `else` expressions are planned.
- **Single-character operators and no exponent notation.** See [Tokens](#tokens).
