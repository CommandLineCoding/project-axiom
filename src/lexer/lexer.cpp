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

void Lexer::skip_whitespace_and_comments() noexcept {
    while (!is_eof()) {
        char c = peek();
        
        // Handle standard whitespace
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        }

        // Handle line comments
        else if (c == '#') {
            while (!is_eof() && peek() != '\n') {
                advance();
            }
        }

        // break the skipping sequence
        else {
            break;
        }
    }
}

Expected<Token> Lexer::next_token() {
    skip_whitespace_and_comments();

    if (is_eof()) {
        return Token{
            .kind = TokenKind::Eof,
            .lexeme = "",
            .location = m_loc
        };
    }

    SourceLocation token_start = m_loc;
    size_t start_cursor = m_cursor;
    
    advance(); 
    
    std::string_view lexeme = m_source.substr(start_cursor, 1);
    return Token{
        .kind = TokenKind::Op,
        .lexeme = lexeme,
        .location = token_start
    };
}

} // namespace axiom