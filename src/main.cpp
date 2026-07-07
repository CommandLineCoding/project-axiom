#include "axiom/lexer.h"
#include "axiom/parser.h"
#include "axiom/ast.h"
#include <print>
#include <utility>

int main() {
    std::println(">_ Running Primary Expression Parser Test");

    std::string_view sample_code = "foo(42.0, x)";
    axiom::Lexer lexer(sample_code);
    axiom::Parser parser(std::move(lexer));

    auto ast_result = parser.parse_top_level_expression();

    if (!ast_result) {
        axiom::report_error(ast_result.error());
        return 1;
    }

    std::println("Parser successfully generated an AST tree configuration structure!");
  
    if (auto* call = dynamic_cast<axiom::CallExpr*>(ast_result->get())) {
        std::println("Root Node: Function call to target -> '{}'", call->callee());
        std::println("Arguments parsed count: {}", call->args().size());
        
        if (auto* arg1 = dynamic_cast<axiom::NumExpr*>(call->args()[0].get())) {
            std::println(" -> Argument 1 Type: Numeric Literal containing -> {}", arg1->val());
        }
        if (auto* arg2 = dynamic_cast<axiom::VarExpr*>(call->args()[1].get())) {
            std::println(" -> Argument 2 Type: Variable Pointer referencing -> '{}'", arg2->name());
        }
    }

    return 0;
}