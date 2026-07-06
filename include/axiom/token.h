#pragma once
#include "axiom/support/source_location.h"
#include <string_view>
#include <format>

namespace axiom {

enum class TokenKind {
    Eof,
    Def,
    Extern,
    Identifier,
    Number,
    Op
};

constexpr std::string_view to_string(TokenKind kind) {
    switch (kind) {
        case TokenKind::Eof:        return "EOF";
        case TokenKind::Def:        return "DEF";
        case TokenKind::Extern:     return "EXTERN";
        case TokenKind::Identifier: return "IDENTIFIER";
        case TokenKind::Number:     return "NUMBER";
        case TokenKind::Op:         return "OPERATOR";
    }
    return "UNKNOWN";
}

struct Token {
    TokenKind kind;
    std::string_view lexeme;
    SourceLocation location;
};

} // namespace axiom

template <>
struct std::formatter<axiom::Token> : std::formatter<std::string_view> {
    auto format(const axiom::Token& tok, std::format_context& ctx) const {
        std::string formatted = std::format("{} ('{}') at {}:{}", 
                                    axiom::to_string(tok.kind), 
                                    tok.lexeme, 
                                    tok.location.line, 
                                    tok.location.column);
        return std::formatter<std::string_view>::format(formatted, ctx);
    }
};