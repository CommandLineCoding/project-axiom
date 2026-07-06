#pragma once
#include <expected>
#include <string>
#include "axiom/support/source_location.h"

namespace axiom {

struct CompilerError {
    std::string message;
    SourceLocation location;
};

template <typename T>
using Expected = std::expected<T, CompilerError>;

void report_error(const CompilerError& err);

} // namespace axiom