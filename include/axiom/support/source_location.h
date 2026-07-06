#pragma once
#include <cstdint>

namespace axiom {

struct SourceLocation {
    uint32_t line = 1;
    uint32_t column = 1;
};

} // namespace axiom