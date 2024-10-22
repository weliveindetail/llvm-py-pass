import ctypes
from llvmlite.binding import ffi
from llvmlite.binding.module import ModuleRef
from llvmlite.ir.transforms import ReplaceCalls

try:
  llvm_mod = ctypes.cast(mod_addr, ffi.LLVMModuleRef)
  llvm_ctx = ctypes.cast(ctx_addr, ffi.LLVMContextRef)

  mod = ModuleRef(llvm_mod, llvm_ctx)
  print(f"\nBefore processing module:\n{str(mod)}")

  rc = ReplaceCalls("foo", "bar")
  rc.visit(mod)

  print(f"\nAfter processing module:\n{str(mod)}")

except Exception as ex:
   print(ex)
