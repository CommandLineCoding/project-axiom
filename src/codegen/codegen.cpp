#include "axiom/codegen.h"
#include "axiom/ast.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <iostream>

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

llvm::Value* BinaryExpr::codegen(CodeGenerator&) { return nullptr; }
llvm::Value* CallExpr::codegen(CodeGenerator&)   { return nullptr; }
llvm::Function* Prototype::codegen(CodeGenerator&) { return nullptr; }
llvm::Function* FuncNode::codegen(CodeGenerator&)  { return nullptr; }

} // namespace axiom