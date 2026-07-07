#include "axiom/codegen.h"
#include "axiom/ast.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <iostream>
#include <vector>

namespace axiom {

CodeGenerator::CodeGenerator() {
    m_context = std::make_unique<llvm::LLVMContext>();
    m_builder = std::make_unique<llvm::IRBuilder<>>(*m_context);
    m_module = std::make_unique<llvm::Module>("AxiomJIT", *m_context);
}

void CodeGenerator::dump() const {
    m_module->print(llvm::errs(), nullptr);
}

llvm::Value* NumExpr::codegen(CodeGenerator& cg) {
    return llvm::ConstantFP::get(cg.context(), llvm::APFloat(m_val));
}

llvm::Value* VarExpr::codegen(CodeGenerator& cg) {
    llvm::Value* val = cg.named_values()[m_name];
    if (!val) {
        std::cerr << "Error: Unknown variable reference name: " << m_name << "\n";
        return nullptr;
    }
    return val;
}

llvm::Value* BinaryExpr::codegen(CodeGenerator& cg) {
    llvm::Value* L = m_lhs->codegen(cg);
    llvm::Value* R = m_rhs->codegen(cg);
    if (!L || !R) return nullptr;

    switch (m_op) {
        case '+':
            return cg.builder().CreateFAdd(L, R, "addtmp");
        case '-':
            return cg.builder().CreateFSub(L, R, "subtmp");
        case '*':
            return cg.builder().CreateFMul(L, R, "multmp");
        case '<': {
            L = cg.builder().CreateFCmpULT(L, R, "cmptmp");
            return cg.builder().CreateUIToFP(L, llvm::Type::getDoubleTy(cg.context()), "booltmp");
        }
        default:
            std::cerr << "Error: Invalid binary operator option: " << m_op << "\n";
            return nullptr;
    }
}

llvm::Value* CallExpr::codegen(CodeGenerator& cg) {
    llvm::Function* callee_fn = cg.module()->getFunction(m_callee);
    if (!callee_fn) {
        std::cerr << "Error: Unknown function referenced: " << m_callee << "\n";
        return nullptr;
    }

    if (callee_fn->arg_size() != m_args.size()) {
        std::cerr << "Error: Incorrect number of arguments passed to function\n";
        return nullptr;
    }

    std::vector<llvm::Value*> args_v;
    for (unsigned i = 0, e = m_args.size(); i != e; ++i) {
        args_v.push_back(m_args[i]->codegen(cg));
        if (!args_v.back()) return nullptr;
    }

    return cg.builder().CreateCall(callee_fn, args_v, "calltmp");
}

llvm::Function* Prototype::codegen(CodeGenerator&) { return nullptr; }
llvm::Function* FuncNode::codegen(CodeGenerator&)  { return nullptr; }

} // namespace axiom