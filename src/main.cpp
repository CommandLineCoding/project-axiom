#include "axiom/support/error.h"
#include <print>
#include <string_view>

axiom::Expected<int> square_positive_number(int num, axiom::SourceLocation loc) {
    if (num < 0) {
        return std::unexpected(axiom::CompilerError{
            .message = "Negative numbers are not allowed in this block",
            .location = loc
        });
    }
    return num * num;
}

int main() {
    std::println(">_ Running Axiom Infrastructure Test");

    axiom::SourceLocation loc1{.line = 10, .column = 5};
    auto success_res = square_positive_number(8, loc1);
    
    if (success_res)
    std::println("Test 1 Passed! Result value: {}", *success_res);
    
    axiom::SourceLocation loc2{.line = 42, .column = 12};
    auto failure_res = square_positive_number(-5, loc2);
    
    if (!failure_res) {
        std::print("Test 2 Output: ");
        axiom::report_error(failure_res.error());
    }

    return 0;
}