#include <pybind11/embed.h>

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include <unistd.h>
#include <limits.h>

namespace py = pybind11;
using namespace py::literals;

bool runPyPassOn(LLVMModuleRef Mod) {
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd))) {
    fprintf(stderr, "Failed to run out-of-tree pass: Cannot obtain current working directory\n");
    return false;
  }

  auto pass_script = std::string(cwd) + "/pass.py";
  std::ifstream file(pass_script);
  if (!file) {
    fprintf(stderr, "Failed to run out-of-tree pass: Cannot find script file %s\n", pass_script.c_str());
    return false;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string script = buffer.str();

  try {
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    fprintf(stderr, "Running out-of-tree pass: %s\n", pass_script.c_str());
    py::module_ sys = py::module_::import("sys");
    auto py_version = py::cast<std::string>(sys.attr("version"));
    fprintf(stderr, "  python version: %s\n", py_version.c_str());
    py::module_ llvmlite = py::module_::import("llvmlite");
    auto llvmlite_version = py::cast<std::string>(llvmlite.attr("__version__"));
    fprintf(stderr, "  llvmlite version: %s\n", llvmlite_version.c_str());

    // Hello World
    // have mod
    // <capsule object NULL at 0x10c14bc00>
    // argument 1: TypeError: expected LP_LLVMModule instance instead of PyCapsule
    auto locals = py::dict("mod"_a=(void *)Mod);
    py::exec(script, py::globals(), locals);
  }
  catch (...) {
    std::abort();
  }

  return true;
}
