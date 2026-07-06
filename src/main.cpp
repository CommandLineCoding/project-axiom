#include <iostream>
#include <llvm/Support/VersionTuple.h>
#include <llvm/Support/raw_ostream.h>

int main() {
    std::cout << "Hello from Project Axiom!" << std::endl;
  
    llvm::VersionTuple llvmVersion(LLVM_VERSION_MAJOR, LLVM_VERSION_MINOR, LLVM_VERSION_PATCH);
    std::cout << "Linked successfully with LLVM Version: " << llvmVersion.getAsString() << std::endl;

    return 0;
}