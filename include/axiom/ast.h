#pragma once
#include <string>
#include <memory>
#include <vector>

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

class BinaryExpr : public ExprAST {
public:
    BinaryExpr(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

    [[nodiscard]] char op() const noexcept { return m_op; }
    [[nodiscard]] const ExprAST* lhs() const noexcept { return m_lhs.get(); }
    [[nodiscard]] const ExprAST* rhs() const noexcept { return m_rhs.get(); }

private:
    char m_op;
    std::unique_ptr<ExprAST> m_lhs;
    std::unique_ptr<ExprAST> m_rhs;
};

class CallExpr : public ExprAST {
public:
    CallExpr(std::string callee, std::vector<std::unique_ptr<ExprAST>> args)
        : m_callee(std::move(callee)), m_args(std::move(args)) {}

    [[nodiscard]] const std::string& callee() const noexcept { return m_callee; }
    [[nodiscard]] const std::vector<std::unique_ptr<ExprAST>>& args() const noexcept { return m_args; }

private:
    std::string m_callee;
    std::vector<std::unique_ptr<ExprAST>> m_args;
};

} // namespace axiom