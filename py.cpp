#include <pybind11/embed.h>

#include <cstdlib>

namespace py = pybind11;

void run() {
  try {
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    py::print("Hello Python! version:");
    py::module_ sys = py::module_::import("sys");
    py::print(sys.attr("version"));

    py::print("Hello llvmlite! version:");
    py::module_ llvmlite = py::module_::import("llvmlite");
    py::print(llvmlite.attr("__version__"));
  } catch (...) {
    exit(1);
  }
}
