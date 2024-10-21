from hashlib import sha256
from llvmlite.binding.common import _encode_string as utf8bytes
from llvmlite.binding.module import ModuleRef

def run(mod: ModuleRef):
    for func in mod.functions:
        if func.name != "main":
            func.name = sha256(utf8bytes(func.name)).hexdigest()
