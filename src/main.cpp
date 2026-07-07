#include "axiom/lexer.h"
#include <print>

int main() {
    std::println(">_ Testing Identifiers & Keywords");
    std::string_view sample_code = "def calculate_area extern pi my_var123";
    axiom::Lexer lexer(sample_code);

    while (true) {
        auto token_res = lexer.next_token();
        if (!token_res) {
            axiom::report_error(token_res.error());
            break;
        }

        std::println("Lexed element -> {}", *token_res);

        if (token_res->kind == axiom::TokenKind::Eof) {
            break;
        }
    }

    return 0;
}