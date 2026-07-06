#include "axiom/support/error.h"
#include <print>
#include <iostream>

namespace axiom {

void report_error(const CompilerError& err) {
    std::println(std::cerr, "axiom:{}:{}: error: {}", 
                 err.location.line, 
                 err.location.column, 
                 err.message);
}

} // namespace axiom