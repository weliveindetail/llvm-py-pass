# llvm-py-pass

Write custom compiler passes in Python

## Example

We want to rename functions `foo` and `bar` in this code at compile-time:
```c
int foo() { return 42; }
int bar() { return 0; }
int main(int argc, char *argv[]) {
  return (argc % 2 == 0) ? foo() : bar();
}
```

Here is a script that does it:
```py
from hashlib import sha256
from llvmlite.binding.common import _encode_string as utf8bytes
from llvmlite.binding.module import ModuleRef

def run(mod: ModuleRef):
    for func in mod.functions:
        if func.name != "main":
            func.name = sha256(utf8bytes(func.name)).hexdigest()
```

This is what we get - with and without the plugin:
```
➜ cd example
➜ /usr/local/opt/llvm@14/bin/clang -O1 test.c -o test
➜ nm test
0000000100000f80 T _bar
0000000100000f70 T _foo
0000000100000f90 T _main
➜ /usr/local/opt/llvm@14/bin/clang -O1 -fpass-plugin=$(pwd)/../build/libllvm-py-pass.dylib test.c -o test
[llvm-py-pass] Obtaining pass plugin info
[llvm-py-pass] Found /path/to/llvm-py-pass/example/run.py
[llvm-py-pass] Adding pass to optimizer pipeline
[llvm-py-pass] Running pass on test.c
➜ nm test
0000000100000f70 T _2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae
0000000100000f80 T _fcde2b2edba56bf408601fb721fe9b5c338d10ee429ea04fae5511b68fbf8fb9
0000000100000f90 T _main
```

This is the IR diff that we dump from the `run.py` wrapper script:
```diff
--- original
+++ processed
@@ -4,12 +4,12 @@
 target triple = "x86_64-apple-macosx14.0.0"
 
 ; Function Attrs: nounwind ssp uwtable
-define i32 @foo() #0 {
+define i32 @"2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae"() #0 {
   ret i32 42
 }
 
 ; Function Attrs: nounwind ssp uwtable
-define i32 @bar() #0 {
+define i32 @fcde2b2edba56bf408601fb721fe9b5c338d10ee429ea04fae5511b68fbf8fb9() #0 {
   ret i32 0
 }
 
@@ -20,11 +20,11 @@
   br i1 %4, label %5, label %7
 
 5:                                                ; preds = %2
-  %6 = call i32 @foo()
+  %6 = call i32 @"2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae"()
   br label %9
 
 7:                                                ; preds = %2
-  %8 = call i32 @bar()
+  %8 = call i32 @fcde2b2edba56bf408601fb721fe9b5c338d10ee429ea04fae5511b68fbf8fb9()
   br label %9
 
 9:                                                ; preds = %7, %5
```

## macOS

### Dependencies
```
➜ brew install cmake
➜ brew install ninja
➜ brew install lit
➜ brew install llvm@14
➜ brew install python@3.10
➜ find /usr/local/opt/python@3.10/ -name *.dylib
/usr/local/opt/python@3.10/Frameworks/Python.framework/Versions/3.10/lib/libpython3.10.dylib
➜ PYHOME=/usr/local/opt/python@3.10/Frameworks/Python.framework/Versions/3.10
➜ $PYHOME/bin/python3.10 -m pip --version
pip 24.2 from /usr/local/lib/python3.10/site-packages/pip (python 3.10)
➜ $PYHOME/bin/python3.10 -m pip install llvmlite
```

### Build and test
```
➜ git clone https://github.com/weliveindetail/llvm-py-pass
➜ PYHOME=/usr/local/opt/python@3.10/Frameworks/Python.framework/Versions/3.10
➜ cmake -GNinja -Bbuild -Sllvm-py-pass -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPYTHON_EXECUTABLE=$PYHOME/bin/python3.10 -DPYTHON_LIBRARY=$PYHOME/lib/libpython3.10.dylib -DLLVM_DIR=/usr/local/opt/llvm@14/lib/cmake/llvm
...
-- Default enable compile_commands.json
-- pybind11 v2.13.6 
-- Found PythonInterp: /usr/local/Cellar/python@3.10/3.10.12_1/Frameworks/Python.framework/Versions/3.10/bin/python3.10 (found suitable version "3.10.12", minimum required is "3.7") 
-- Found PythonLibs: /usr/local/Cellar/python@3.10/3.10.12_1/Frameworks/Python.framework/Versions/3.10/lib/libpython3.10.dylib
..
-- Found llvm-lit: /usr/local/bin/lit
...
-- Build files have been written to: /path/to/build
➜ ninja -C build check
ninja: Entering directory `build'
[3/4] Running llvm-py-pass tests
Running tests against tools from: /usr/local/opt/llvm@14/bin
Testing plugin file: /path/to/build/libllvm-py-pass.dylib
-- Testing: 4 tests, 4 workers --
PASS: llvm-py-pass tests :: hello/hello-opt.ll (1 of 4)
PASS: llvm-py-pass tests :: hello/hello-clang.c (2 of 4)
PASS: llvm-py-pass tests :: rename/rename-opt.ll (3 of 4)
PASS: llvm-py-pass tests :: rename/rename-clang.c (4 of 4)

Testing Time: 2.01s
  Passed: 4
```
