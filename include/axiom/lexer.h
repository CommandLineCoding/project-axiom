#pragma once
#include <string_view>
#include "axiom/token.h"
#include "axiom/support/error.h"

namespace axiom {
    class Lexer {
    public:
        explicit Lexer(std::string_view source) : m_source(source) {}
        Expected<Token> next_token();

    private:
        [[nodiscard]] bool is_eof() const noexcept;
        [[nodiscard]] char peek() const noexcept;
        char advance() noexcept;
        void skip_whitespace_and_comments() noexcept;

        std::string_view m_source;
        size_t m_cursor = 0;
        SourceLocation m_loc{ .line = 1, .column = 1 };
    };
} // namespace axiom