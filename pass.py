print("Hello World")

if mod:
  print("have mod")
  print(mod)

try:
  from llvmlite.binding import ffi
  print(ffi.lib.LLVMPY_GetModuleName(mod))

  from llvmlite.binding.module import ModuleRef
  mod_ref = ModuleRef(mod, None)

  from llvmlite.ir.transforms import ReplaceCalls
  rc = ReplaceCalls("before", "after")
  rc.visit(mod_ref)

except Exception as ex:
   print(ex)
