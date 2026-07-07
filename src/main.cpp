#include "axiom/codegen.h"
#include "axiom/ast.h"
#include <print>

int main() {
    std::println(">_ Testing Codegen Base Infrastructure");

    axiom::CodeGenerator cg;
    axiom::NumExpr numeric_literal(13.37);
    auto* llvm_val = numeric_literal.codegen(cg);

    if (llvm_val) {
        std::println("LLVM Integration Operational! Initial generated constant:");
        cg.dump();
    } else {
        std::println("Failed to emit valid LLVM structures.");
    }

    return 0;
}