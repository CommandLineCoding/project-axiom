#include "axiom/parser.h"
#include <print>
#include <utility>

namespace axiom {

Parser::Parser(Lexer lexer) : m_lexer(std::move(lexer)) {
    consume();
}

void Parser::consume() noexcept {
    auto next = m_lexer.next_token();
    if (next) {
        m_current_tok = *next;
    } else {        
        m_current_tok = Token{
            .kind = TokenKind::Eof,
            .lexeme = "",
            .location = m_lexer.next_token().error().location
        };
    }
}

void Parser::parse_stub() {
    std::println("Parser initialized successfully! Initial lookahead -> {}", m_current_tok);
}

} // namespace axiom