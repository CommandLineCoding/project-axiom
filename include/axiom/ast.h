#pragma once
#include <string>
#include <memory>

namespace axiom {

class ExprAST {
public:
    virtual ~ExprAST() = default;
};

class NumExpr : public ExprAST {
public:
    explicit NumExpr(double val) : m_val(val) {}
    [[nodiscard]] double val() const noexcept { return m_val; }

private:
    double m_val;
};

class VarExpr : public ExprAST {
public:
    explicit VarExpr(std::string name) : m_name(std::move(name)) {}
    
    [[nodiscard]] const std::string& name() const noexcept { return m_name; }

private:
    std::string m_name;
};

} // namespace axiom