#include "axiom/lexer.h"
#include <print>

namespace axiom {

bool Lexer::is_eof() const noexcept {
    return m_cursor >= m_source.size();
}

char Lexer::peek() const noexcept {
    if (is_eof()) return '\0';
    return m_source[m_cursor];
}

char Lexer::advance() noexcept {
    if (is_eof()) return '\0';
    
    char current = m_source[m_cursor++];
    
    if (current == '\n') {
        m_loc.line++;
        m_loc.column = 1;
    } else {
        m_loc.column++;
    }
    
    return current;
}

    Expected<Token> Lexer::next_token() {
        while (!is_eof()) {
            char c = peek();
            std::print("{}", c); 
            advance();
        }

        return Token{
            .kind = TokenKind::Eof,
            .lexeme = "",
            .location = m_loc
        };
    }

} // namespace axiom