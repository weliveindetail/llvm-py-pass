Build llvmlite and install locally:
```
➜ git clone https://github.com/numba/llvmlite
➜ cd llvmlite
➜ git switch release0.43
➜ git diff | cat
diff --git a/llvmlite/utils.py b/llvmlite/utils.py
index e07ecd3..bf914fb 100644
--- a/llvmlite/utils.py
+++ b/llvmlite/utils.py
@@ -11,7 +11,7 @@ def get_library_name():
     """
     if os.name == 'posix':
         if sys.platform == 'darwin':
-            return 'libllvmlite.dylib'
+            return 'build/libllvmlite.dylib'
         else:
             return 'libllvmlite.so'
     else:
diff --git a/setup.py b/setup.py
index 0823599..b2b6350 100644
--- a/setup.py
+++ b/setup.py
@@ -89,7 +89,7 @@ class LlvmliteBuildExt(build_ext):
 
     def run(self):
         build_ext.run(self)
-        build_library_files(self.dry_run)
+        #build_library_files(self.dry_run)
         # HACK: this makes sure the library file (which is large) is only
         # included in binary builds, not source builds.
         from llvmlite.utils import get_library_files
➜ cmake -S ffi -B build -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLLVM_DIR=$(pwd)/build-llvm14/lib/cmake/llvm -DCMAKE_CXX_FLAGS=-std=c++14
➜ python3.10 -m pip install -e .
Obtaining file:///pwd/llvmlite
  Preparing metadata (setup.py) ... done
Installing collected packages: llvmlite
  Running setup.py develop for llvmlite
Successfully installed llvmlite-0.44.0.dev0+24.g5c181ff.dirty
➜ python3.10
>>> import sys
>>> sys.version
'3.10.12 (main, Jun 20 2023, 17:00:24) [Clang 14.0.3 (clang-1403.0.22.14.1)]'
>>> import llvmlite
>>> llvmlite.__version__
'0.43.0-dirty'
>>> exit()
```

Build the pass plugin:
```
➜ tar -xzf pybind11.tar.gz
➜ ls -l $(which python3.10)
/usr/local/bin/python3.10 -> ../Cellar/python@3.10/3.10.12_1/bin/python3.10
➜ PYHOME=/usr/local/Cellar/python@3.10/3.10.12_1/Frameworks/Python.framework/Versions/3.10
➜ git clone llvm-py-pass
➜ cmake -GNinja -B build-llvm-py-pass -S llvm-py-pass -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DPYTHON_EXECUTABLE=$PYHOME/bin/python3.10 -DPYTHON_LIBRARY=$PYHOME/lib/libpython3.10.dylib -Dpybind11_DIR=$(pwd)/share/cmake/pybind11 -DLLVM_DIR=$(pwd)/build-llvm14/lib/cmake/llvm
➜ ninja -C build-llvm-py-pass
[2/2] Linking CXX shared library libllvm-py-pass.dylib
```

Run the pass plugin:
```
➜ build-llvm14/bin/clang --version
clang version 14.0.6 (https://github.com/llvm/llvm-project f28c006a5895fc0e329fe15fead81e37457cb1d1)
Target: x86_64-apple-darwin23.5.0
Thread model: posix
InstalledDir: /pwd/llvm-py-pass/build-llvm14/bin
➜ build-llvm14/bin/clang -isysroot $(xcrun --show-sdk-path) -fpass-plugin=$(pwd)/build/libllvm-py-pass.dylib -O1 test.c
➜ ../llvm-project/build-release14-relwithdebinfo/bin/clang -isysroot $(xcrun --show-sdk-path) -fplugin=$(pwd)/build/libllvm-py-pass.dylib -fpass-plugin=$(pwd)/build/libllvm-py-pass.dylib -O1 test.c
Obtaining pass plugin info
Adding pass to optimizer pipeline
Running pass on function: main
sys.version
3.10.12 (main, Jun 20 2023, 17:00:24) [Clang 14.0.3 (clang-1403.0.22.14.1)]
llvmlite.__version__:
0.43.0-dirty
```
