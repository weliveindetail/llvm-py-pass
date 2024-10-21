#include <llvm/IR/Function.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

using namespace llvm;

extern void run();

namespace {

bool runBye(Function &F) {
  printf("Running pass on function: %s\n", F.getName().data());
  run();
  return true;
}

struct Bye : PassInfoMixin<Bye> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    return runBye(F) ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // namespace

/* New PM Registration */
llvm::PassPluginLibraryInfo getByePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Bye", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerVectorizerStartEPCallback(
                [](llvm::FunctionPassManager &PM, OptimizationLevel Level) {
                  printf("Adding pass to optimizer pipeline\n");
                  PM.addPass(Bye());
                });
            PB.registerPipelineParsingCallback(
                [](StringRef Name, llvm::FunctionPassManager &PM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "goodbye") {
                    printf("Adding pass to default pipeline\n");
                    PM.addPass(Bye());
                    return true;
                  }
                  return false;
                });
          }};
}

#ifndef LLVM_BYE_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  printf("Obtaining pass plugin info\n");
  return getByePluginInfo();
}
#endif
