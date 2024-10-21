from ctypes import cast
from hashlib import sha256
from llvmlite.binding import ffi
from llvmlite.binding.common import _encode_string as utf8bytes
from llvmlite.binding.module import ModuleRef

# mod_addr and ctx_addr are injected from py.cpp
llvm_mod = cast(mod_addr, ffi.LLVMModuleRef)
llvm_ctx = cast(ctx_addr, ffi.LLVMContextRef)
mod = ModuleRef(llvm_mod, llvm_ctx)

for func in mod.functions:
    if func.name != "main":
        func.name = sha256(utf8bytes(func.name)).hexdigest()
