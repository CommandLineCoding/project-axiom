#include "axiom/lexer.h"
#include "axiom/parser.h"
#include "axiom/ast.h"
#include <print>
#include <utility>

int main() {
    std::println(">_ Running Operator Precedence Parser Test");

    std::string_view sample_code = "x + 4.0 * y";
    axiom::Lexer lexer(sample_code);
    axiom::Parser parser(std::move(lexer));

    auto ast_result = parser.parse_top_level_expression();

    if (!ast_result) {
        axiom::report_error(ast_result.error());
        return 1;
    }

    std::println("AST successfully generated with operator weights calculated!");
    
    if (auto* root_bin = dynamic_cast<axiom::BinaryExpr*>(ast_result->get())) {
        std::println("Root Operation Node: '{}'", root_bin->op());
        
        if (auto* left = dynamic_cast<const axiom::VarExpr*>(root_bin->lhs())) {
            std::println(" -> Left side is variable: '{}'", left->name());
        }
        
        if (auto* right_bin = dynamic_cast<const axiom::BinaryExpr*>(root_bin->rhs())) {
            std::println(" -> Right side correctly nested tighter operation: '{}'", right_bin->op());
            if (auto* inner_num = dynamic_cast<const axiom::NumExpr*>(right_bin->lhs())) {
                std::println("     -> Inner Left: {}", inner_num->val());
            }
        }
    }

    return 0;
}