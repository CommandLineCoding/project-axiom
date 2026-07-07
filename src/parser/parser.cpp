#include "axiom/parser.h"
#include <charconv>
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

int Parser::get_tok_precedence() noexcept {
    if (m_current_tok.kind != TokenKind::Op) return -1;
    
    char op = m_current_tok.lexeme[0];
    switch (op) {
        case '<': return 10;
        case '+': return 20;
        case '-': return 20;
        case '*': return 40;
        default:  return -1;
    }
}

Expected<std::unique_ptr<ExprAST>> Parser::parse_number_expr() {
    double val = 0.0;
    auto [ptr, ec] = std::from_chars(m_current_tok.lexeme.data(), 
                                     m_current_tok.lexeme.data() + m_current_tok.lexeme.size(), 
                                     val);
    if (ec != std::errc{}) {
        return std::unexpected(CompilerError{
            .message = "Failed to parse numeric literal values",
            .location = m_current_tok.location
        });
    }
    auto node = std::make_unique<NumExpr>(val);
    consume(); 
    return node;
}

Expected<std::unique_ptr<ExprAST>> Parser::parse_identifier_expr() {
    std::string id_name(m_current_tok.lexeme);
    consume(); 

    if (m_current_tok.lexeme != "(") {
        return std::make_unique<VarExpr>(id_name);
    }
    consume(); 
    std::vector<std::unique_ptr<ExprAST>> args;
    
    if (m_current_tok.lexeme != ")") {
        while (true) {
            auto arg = parse_expression();
            if (!arg) return arg;
            args.push_back(std::move(*arg));

            if (m_current_tok.lexeme == ")") break;

            if (m_current_tok.lexeme != ",") {
                return std::unexpected(CompilerError{
                    .message = "Expected ',' or ')' inside parameter collection",
                    .location = m_current_tok.location
                });
            }
            consume(); 
        }
    }
    consume(); 
    return std::make_unique<CallExpr>(id_name, std::move(args));
}

Expected<std::unique_ptr<ExprAST>> Parser::parse_paren_expr() {
    consume(); 
    auto expr = parse_expression();
    if (!expr) return expr;

    if (m_current_tok.lexeme != ")") {
        return std::unexpected(CompilerError{
            .message = "Expected matching closing parenthesis ')'",
            .location = m_current_tok.location
        });
    }
    consume(); 
    return expr;
}

Expected<std::unique_ptr<ExprAST>> Parser::parse_primary() {
    switch (m_current_tok.kind) {
        case TokenKind::Identifier: return parse_identifier_expr();
        case TokenKind::Number:     return parse_number_expr();
        case TokenKind::Op:
            if (m_current_tok.lexeme == "(") return parse_paren_expr();
            break;
        default:
            break;
    }
    return std::unexpected(CompilerError{
        .message = "Unknown token type encountered when expecting an expression node",
        .location = m_current_tok.location
    });
}


Expected<std::unique_ptr<ExprAST>> Parser::parse_bin_op_rhs(int expr_prec, std::unique_ptr<ExprAST> lhs) {
    while (true) {
        int tok_prec = get_tok_precedence();
        if (tok_prec < expr_prec) return lhs;

        char bin_op = m_current_tok.lexeme[0];
        consume();

        auto rhs = parse_primary();
        if (!rhs) return rhs;

        int next_prec = get_tok_precedence();
        if (tok_prec < next_prec) {
            rhs = parse_bin_op_rhs(tok_prec + 1, std::move(*rhs));
            if (!rhs) return rhs;
        }

        lhs = std::make_unique<BinaryExpr>(bin_op, std::move(lhs), std::move(*rhs));
    }
}

Expected<std::unique_ptr<ExprAST>> Parser::parse_expression() {
    auto lhs = parse_primary();
    if (!lhs) return lhs;
    return parse_bin_op_rhs(0, std::move(*lhs));
}

Expected<std::unique_ptr<ExprAST>> Parser::parse_top_level_expression() {
    return parse_expression();
}

} // namespace axiom