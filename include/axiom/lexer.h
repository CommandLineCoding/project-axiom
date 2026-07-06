#pragma once
#include <string_view>
#include "axiom/support/source_location.h"

namespace axiom {

class Lexer {
public:
    explicit Lexer(std::string_view source) : m_source(source) {}

private:
    std::string_view m_source;
    size_t m_cursor = 0;
    SourceLocation m_loc{};
};

} // namespace axiom