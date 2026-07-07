#pragma once
#include <string>
#include <memory>
#include <vector>

namespace llvm {
class Value;
class Function;
} // namespace llvm

namespace axiom {
class CodeGenerator;

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual llvm::Value* codegen(CodeGenerator& cg) = 0;
};

class NumExpr : public ExprAST {
public:
    explicit NumExpr(double val) : m_val(val) {}
    [[nodiscard]] double val() const noexcept { return m_val; }
    
    llvm::Value* codegen(CodeGenerator& cg) override;

private:
    double m_val;
};

class VarExpr : public ExprAST {
public:
    explicit VarExpr(std::string name) : m_name(std::move(name)) {}
    [[nodiscard]] const std::string& name() const noexcept { return m_name; }
    
    llvm::Value* codegen(CodeGenerator& cg) override;

private:
    std::string m_name;
};

class BinaryExpr : public ExprAST {
public:
    BinaryExpr(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

    [[nodiscard]] char op() const noexcept { return m_op; }
    
    llvm::Value* codegen(CodeGenerator& cg) override;

private:
    char m_op;
    std::unique_ptr<ExprAST> m_lhs;
    std::unique_ptr<ExprAST> m_rhs;
};

class CallExpr : public ExprAST {
public:
    CallExpr(std::string callee, std::vector<std::unique_ptr<ExprAST>> args)
        : m_callee(std::move(callee)), m_args(std::move(args)) {}

    llvm::Value* codegen(CodeGenerator& cg) override;

private:
    std::string m_callee;
    std::vector<std::unique_ptr<ExprAST>> m_args;
};

class Prototype {
public:
    Prototype(std::string name, std::vector<std::string> args)
        : m_name(std::move(name)), m_args(std::move(args)) {}

    [[nodiscard]] const std::string& name() const noexcept { return m_name; }
    
    llvm::Function* codegen(CodeGenerator& cg);

private:
    std::string m_name;
    std::vector<std::string> m_args;
};

class FuncNode {
public:
    FuncNode(std::unique_ptr<Prototype> proto, std::unique_ptr<ExprAST> body)
        : m_proto(std::move(proto)), m_body(std::move(body)) {}

    llvm::Function* codegen(CodeGenerator& cg);

private:
    std::unique_ptr<Prototype> m_proto;
    std::unique_ptr<ExprAST> m_body;
};

} // namespace axiom