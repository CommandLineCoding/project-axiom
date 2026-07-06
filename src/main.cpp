#include "axiom/lexer.h"
#include <print>

int main() {
    std::println(">_ Testing Lexer Navigation Skeleton");

    std::string_view sample_code = "def foo\n\
    sss";
    axiom::Lexer lexer(sample_code);

    auto token_result = lexer.next_token();
    
    if (token_result) std::println("\nSuccessfully retrieved token: {}", *token_result);
    else axiom::report_error(token_result.error());
    
    return 0;
}