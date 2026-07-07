#include "axiom/lexer.h"
#include "axiom/parser.h"
#include "axiom/ast.h"
#include <print>
#include <utility>

int main() {
    std::println(">_ Running Full Grammar Frontend Integration Test");

    std::string_view source_program = 
        "extern sin(x)\n"
        "def volume(r) 4.18 * r * r\n"
        "volume(2.5)";

    axiom::Lexer lexer(source_program);
    axiom::Parser parser(std::move(lexer));

    while (!parser.is_eof()) {
        switch (parser.current_tok_kind()) {
            case axiom::TokenKind::Extern: {
                auto ext_node = parser.parse_extern();
                if (ext_node) {
                    std::println("Parsed 'extern' mapping target: {}", (*ext_node)->name());
                } else {
                    axiom::report_error(ext_node.error());
                    return 1;
                }
                break;
            }
            case axiom::TokenKind::Def: {
                auto func_node = parser.parse_definition();
                if (func_node) {
                    std::println("Parsed 'def' code block: {}", (*func_node)->proto()->name());
                } else {
                    axiom::report_error(func_node.error());
                    return 1;
                }
                break;
            }
            default: {
                auto anon_node = parser.parse_top_level_expression();
                if (anon_node) {
                    std::println("Parsed standalone expression wrapped into: {}", (*anon_node)->proto()->name());
                } else {
                    axiom::report_error(anon_node.error());
                    return 1;
                }
                break;
            }
        }
    }

    std::println("Parser Frontend completed successfully!");
    return 0;
}