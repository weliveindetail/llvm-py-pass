import os
import lit
import sys
import platform
import subprocess
from lit.llvm import llvm_config

config.name = "llvm-py-pass tests"
config.suffixes = ['.c', '.cpp', '.ll']
config.test_format = lit.formats.ShTest(True)
config.test_source_root = os.path.dirname(__file__)

llvm_config.add_tool_substitutions(["opt", "clang", "clang++", "llvm-nm", "FileCheck", "count", "not"], config.llvm_tools_dir)

plugin_file = os.path.join(config.plugin_dir, 'libllvm-py-pass' + config.llvm_plugin_suffix)
config.substitutions.append((r'%plugin', plugin_file))
if sys.platform == 'darwin':
  config.substitutions.append((r'%exeflags', "-isysroot " + config.isysroot))
else:
  config.substitutions.append((r'%exeflags', ""))

print("Running tests against tools from:", config.llvm_tools_dir)
print("Testing plugin file:", plugin_file)
