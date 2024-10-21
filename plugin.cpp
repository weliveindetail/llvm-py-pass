#include <llvm/IR/Function.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

#include <cstdio>
#include <mutex>

using namespace llvm;

extern bool runPyPassOn(LLVMModuleRef);

namespace {

static std::once_flag register_once;

struct PyPass : PassInfoMixin<PyPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    LLVMModuleRef MR = wrap(&M);
    return runPyPassOn(MR) ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static PassPluginLibraryInfo pluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "PyPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
              PB.registerPipelineEarlySimplificationEPCallback(
                  [](ModulePassManager &MPM, OptimizationLevel opt) {
                    std::call_once(register_once, [&]() {
                      printf("Adding pass to optimizer pipeline\n");
                      MPM.addPass(PyPass());
                      return true;
                    });
                  });
            }};
  }
};

} // namespace

#ifndef LLVM_BYE_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  printf("Obtaining pass plugin info\n");
  return PyPass::pluginInfo();
}
#endif
