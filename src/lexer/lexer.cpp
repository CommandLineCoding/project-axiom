#include "axiom/lexer.h"
#include <print>
#include <cctype>

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
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '#') {
            while (!is_eof() && peek() != '\n') {
                advance();
            }
        } else {
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

    char c = peek();

    if (std::isalpha(c) || c == '_') {
        SourceLocation token_start = m_loc;
        size_t start_cursor = m_cursor;

        while (!is_eof() && (std::isalnum(peek()) || peek() == '_'))
        advance();
        
        std::string_view lexeme = m_source.substr(start_cursor, m_cursor - start_cursor);
        
        TokenKind kind = TokenKind::Identifier;
        if (lexeme == "def") {
            kind = TokenKind::Def;
        } else if (lexeme == "extern") {
            kind = TokenKind::Extern;
        }

        return Token{
            .kind = kind,
            .lexeme = lexeme,
            .location = token_start
        };
    }

    if (std::isdigit(c) || c == '.') {
        
        if (c == '.' && (m_cursor + 1 >= m_source.size() || !std::isdigit(m_source[m_cursor + 1]))) {
            
        } else {
            SourceLocation token_start = m_loc;
            size_t start_cursor = m_cursor;

            while (!is_eof() && std::isdigit(peek())) {
                advance();
            }

            if (!is_eof() && peek() == '.') {
                advance();

                while (!is_eof() && std::isdigit(peek())) {
                    advance();
                }
            }

            std::string_view lexeme = m_source.substr(start_cursor, m_cursor - start_cursor);
            return Token{
                .kind = TokenKind::Number,
                .lexeme = lexeme,
                .location = token_start
            };
        }
    }

    SourceLocation token_start = m_loc;
    size_t start_cursor = m_cursor;
    advance(); 
    
    return Token{
        .kind = TokenKind::Op,
        .lexeme = m_source.substr(start_cursor, 1),
        .location = token_start
    };
}

} // namespace axiom