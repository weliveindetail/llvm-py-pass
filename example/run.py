import ctypes
import llvmlite
import sys

from difflib import unified_diff
from llvmlite.binding import ffi
from llvmlite.binding.module import ModuleRef

import rename

try:
    print(f"Python version: {sys.version}")
    print(f"llvmlite version: {llvmlite.__version__}")

    # mod_addr and ctx_addr are injected from py.cpp
    llvm_mod = ctypes.cast(mod_addr, ffi.LLVMModuleRef)
    llvm_ctx = ctypes.cast(ctx_addr, ffi.LLVMContextRef)
    mod = ModuleRef(llvm_mod, llvm_ctx)

    original = str(mod)

    rename.run(mod)
    processed = str(mod)
    before = original.splitlines()
    after = processed.splitlines()
    diff = unified_diff(before, after, 'original', 'processed', lineterm='')
    output = "\n".join(diff)
    if len(output) == 0:
        print("NO CHANGES")
    else:
        print(output)

except Exception as ex:
     print(ex)
