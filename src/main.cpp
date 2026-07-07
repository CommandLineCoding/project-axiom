#include "axiom/codegen.h"
#include "axiom/ast.h"
#include <print>
#include <memory>

int main() {
    std::println(">_ Testing Expression & Operator Codegen");

    axiom::CodeGenerator cg;

    auto lhs_mul = std::make_unique<axiom::NumExpr>(5.0);
    auto rhs_mul = std::make_unique<axiom::NumExpr>(2.0);
    auto mul_expr = std::make_unique<axiom::BinaryExpr>('*', std::move(lhs_mul), std::move(rhs_mul));

    auto lhs_add = std::make_unique<axiom::NumExpr>(10.0);
    auto full_expr = std::make_unique<axiom::BinaryExpr>('+', std::move(lhs_add), std::move(mul_expr));
 
    auto* llvm_val = full_expr->codegen(cg);

    if (llvm_val) {
        std::println("Success! Expression compiled cleanly without validation faults.");
    } else {
        std::println("Failed to compile target expression branches.");
    }

    return 0;
}