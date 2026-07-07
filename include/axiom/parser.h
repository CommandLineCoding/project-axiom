#pragma once
#include "axiom/lexer.h"
#include "axiom/token.h"
#include "axiom/ast.h"
#include "axiom/support/error.h"
#include <memory>

namespace axiom {

class Parser {
public:
    explicit Parser(Lexer lexer);

    Expected<std::unique_ptr<ExprAST>> parse_top_level_expression();

private:
    void consume() noexcept;
    
    Expected<std::unique_ptr<ExprAST>> parse_primary();
    Expected<std::unique_ptr<ExprAST>> parse_number_expr();
    Expected<std::unique_ptr<ExprAST>> parse_identifier_expr();
    Expected<std::unique_ptr<ExprAST>> parse_paren_expr();
    Expected<std::unique_ptr<ExprAST>> parse_expression();

    [[nodiscard]] int get_tok_precedence() noexcept;
    Expected<std::unique_ptr<ExprAST>> parse_bin_op_rhs(int expr_prec, std::unique_ptr<ExprAST> lhs);

    Lexer m_lexer;
    Token m_current_tok{};
};

} // namespace axiom