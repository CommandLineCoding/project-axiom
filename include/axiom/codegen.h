#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <memory>
#include <string>
#include <map>

namespace axiom {

class CodeGenerator {
public:
    CodeGenerator();
    ~CodeGenerator() = default;

    [[nodiscard]] llvm::LLVMContext& context() noexcept { return *m_context; }
    [[nodiscard]] llvm::IRBuilder<>& builder() noexcept { return *m_builder; }
    [[nodiscard]] llvm::Module* module() noexcept { return m_module.get(); }
    [[nodiscard]] std::map<std::string, llvm::Value*>& named_values() noexcept { return m_named_values; }
    void dump() const;

private:
    std::unique_ptr<llvm::LLVMContext> m_context;
    std::unique_ptr<llvm::IRBuilder<>> m_builder;
    std::unique_ptr<llvm::Module> m_module;
    std::map<std::string, llvm::Value*> m_named_values;
};

} // namespace axiom