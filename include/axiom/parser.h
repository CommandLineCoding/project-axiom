#pragma once
#include "axiom/lexer.h"
#include "axiom/token.h"
#include "axiom/support/error.h"

namespace axiom {

class Parser {
public:
    explicit Parser(Lexer lexer);
    void parse_stub();

private:
    void consume() noexcept;

    Lexer m_lexer;
    Token m_current_tok{};
};

} // namespace axiom