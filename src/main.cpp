#include "axiom/token.h"
#include <print>
#include <vector>

int main() {
    std::println("--- Running Axiom Token & Formatter Test ---");

    std::vector<axiom::Token> mock_tokens = {
        { axiom::TokenKind::Def,        "def",    { .line = 1, .column = 1 } },
        { axiom::TokenKind::Identifier, "foo",    { .line = 1, .column = 5 } },
        { axiom::TokenKind::Op,         "(",      { .line = 1, .column = 8 } },
        { axiom::TokenKind::Identifier, "x",      { .line = 1, .column = 9 } },
        { axiom::TokenKind::Op,         ")",      { .line = 1, .column = 10 } },
        { axiom::TokenKind::Number,     "42.0",   { .line = 2, .column = 5 } },
        { axiom::TokenKind::Eof,        "",       { .line = 2, .column = 11 } }
    };

    for (const auto& token : mock_tokens) {
        std::println("Token status -> {}", token);
    }

    return 0;
}