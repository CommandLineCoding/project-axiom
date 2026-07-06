#include "axiom/lexer.h"
#include <print>

int main() {
    std::println(">_ Running Axiom Core Wiring Test");

    std::string_view code = "def foo(x) x + 1;";
    axiom::Lexer lexer(code);

    std::println(">_ DONE!");
    return 0;
}