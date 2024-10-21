#include <pybind11/embed.h>

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

namespace py = pybind11;
using namespace py::literals;

bool runPyPassOn(const std::string &Script, LLVMModuleRef Mod,
                 LLVMContextRef Ctx) {
  py::scoped_interpreter interp;
  try {
    py::dict vars = py::globals();
    vars["mod_addr"_s] = py::reinterpret_steal<py::object>(PyLong_FromVoidPtr(Mod));
    vars["ctx_addr"_s] = py::reinterpret_steal<py::object>(PyLong_FromVoidPtr(Ctx));
    py::exec(Script, vars, vars);
  } catch (const std::exception &ex) {
    fprintf(stderr, "llvm-py-pass: Script failed with error:\n%s\n", ex.what());
    return false;
  }

  return true;
}
