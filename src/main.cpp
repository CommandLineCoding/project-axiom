#include "axiom/lexer.h"
#include "axiom/parser.h"
#include <print>

int main() {
    std::println(">_ Testing Parser Wiring");

    std::string_view sample_code = "def foo(x) x;";
    axiom::Lexer lexer(sample_code);
    axiom::Parser parser(std::move(lexer));

    parser.parse_stub();

    return 0;
}