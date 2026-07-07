#include "axiom/ast.h"
#include <print>
#include <vector>
#include <memory>

int main() {
    std::println(">_ Testing AST Initial Nodes");

    std::vector<std::unique_ptr<axiom::ExprAST>> expressions;

    expressions.push_back(std::make_unique<axiom::NumExpr>(3.14));
    expressions.push_back(std::make_unique<axiom::VarExpr>("my_variable"));

    for (const auto& expr : expressions) {
        if (auto* num = dynamic_cast<axiom::NumExpr*>(expr.get())) {
            std::println("Found Literal Number Node: {}", num->val());
        } else if (auto* var = dynamic_cast<axiom::VarExpr*>(expr.get())) {
            std::println("Found Variable Reference Node: {}", var->name());
        }
    }

    std::println("AST memory cleaned up automatically via unique_ptr.");
    return 0;
}