#include "axiom/ast.h"
#include <print>
#include <vector>
#include <memory>

int main() {
    std::println(">_ Testing Function Structural Nodes");

    std::vector<std::string> proto_args = {"x"};
    auto proto = std::make_unique<axiom::Prototype>("identity", std::move(proto_args));

    auto body = std::make_unique<axiom::VarExpr>("x");

    axiom::FuncNode complete_function(std::move(proto), std::move(body));

    std::println("Function target name: {}", complete_function.proto()->name());
    std::println("Arguments declared count: {}", complete_function.proto()->args().size());
    std::println("First arg descriptor: '{}'", complete_function.proto()->args()[0]);

    if (auto* body_var = dynamic_cast<const axiom::VarExpr*>(complete_function.body())) {
        std::println("Function body verified as variable reference returning: '{}'", body_var->name());
    }

    return 0;
}