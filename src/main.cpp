#include "axiom/ast.h"
#include <print>
#include <vector>
#include <memory>

int main() {
    std::println(">_ Testing Complex AST Branch Nodes");

    auto lhs_op = std::make_unique<axiom::NumExpr>(4.0);
    auto rhs_op = std::make_unique<axiom::VarExpr>("y");
    auto bin_expr = std::make_unique<axiom::BinaryExpr>('+', std::move(lhs_op), std::move(rhs_op));

    std::vector<std::unique_ptr<axiom::ExprAST>> call_args;
    call_args.push_back(std::make_unique<axiom::VarExpr>("x"));
    call_args.push_back(std::move(bin_expr));

    auto root_call = std::make_unique<axiom::CallExpr>("multiply", std::move(call_args));

    std::println("Root Call Callee target: {}", root_call->callee());
    std::println("Number of arguments passed: {}", root_call->args().size());

    if (auto* inner_bin = dynamic_cast<const axiom::BinaryExpr*>(root_call->args()[1].get())) {
        std::println("Argument 2 is an operational branch evaluated using: '{}'", inner_bin->op());
    }

    return 0;
}