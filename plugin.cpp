#include <llvm/IR/Function.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace llvm;

extern bool runPyPassOn(const std::string &, LLVMModuleRef, LLVMContextRef);

namespace {

template <typename PredFn> static void visitScriptPaths(PredFn &&Pred) {
  SmallString<256> dir;
  sys::fs::current_path(dir);
  std::vector<std::string> paths;
  while (true) {
    if (Pred(dir.str()))
      return;
    if (!sys::path::has_parent_path(dir))
      break;
    dir = sys::path::parent_path(dir);
  }
}

static std::string Script;
static std::string ScriptPath;

struct PyPass : PassInfoMixin<PyPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    LLVMModuleRef Mod = wrap(&M);
    LLVMContextRef Ctx = wrap(&M.getContext());
    fprintf(stderr, "[llvm-py-pass] Running pass on %s\n",
            M.getModuleIdentifier().c_str());
    SmallString<256> CWD;
    sys::fs::current_path(CWD);
    sys::fs::set_current_path(ScriptPath);
    bool Changed = runPyPassOn(Script, Mod, Ctx);
    sys::fs::set_current_path(CWD);
    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  if (Script.empty()) {
    fprintf(stderr, "[llvm-py-pass] Obtaining pass plugin info\n");
    visitScriptPaths([&](Twine path) {
      auto file = path + "/run.py";
      if (sys::fs::exists(path)) {
        std::ifstream stream(file.str());
        std::stringstream buffer;
        buffer << stream.rdbuf();
        Script = buffer.str();
        ScriptPath = path.str();
        fprintf(stderr, "[llvm-py-pass] Found %s/run.py\n", ScriptPath.c_str());
        return true; // found, done
      }
      return false; // check next location
    });
  }

  if (Script.empty()) {
    fprintf(stderr, "[llvm-py-pass] Skipped: Cannot find script 'run.py' in "
                    "any of the following locations\n");
    visitScriptPaths([&](Twine path) {
      fprintf(stderr, "  %s\n", path.str().c_str());
      return false;
    });
    return {LLVM_PLUGIN_API_VERSION, "llvm-py-pass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {}};
  }

  return {LLVM_PLUGIN_API_VERSION, "llvm-py-pass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineEarlySimplificationEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel opt) {
                  fprintf(stderr,
                          "[llvm-py-pass] Adding pass to optimizer pipeline\n");
                  MPM.addPass(PyPass());
                  return true;
                });
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement> InnerPipeline) {
                  if (Name == "llvm-py-pass") {
                    fprintf(stderr,
                            "[llvm-py-pass] Adding pass llvm-py-pass\n");
                    MPM.addPass(PyPass());
                    return true;
                  }
                  return false;
                });
          }};
}
