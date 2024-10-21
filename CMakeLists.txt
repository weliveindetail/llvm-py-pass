cmake_minimum_required(VERSION 3.20)
project(llvm-py-pass)

find_package(pybind11 REQUIRED)
find_package(LLVM REQUIRED)

add_library(llvm-py-pass SHARED plugin.cpp py.cpp)
target_include_directories(llvm-py-pass PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(llvm-py-pass PRIVATE pybind11::embed LLVMipo)

set_source_files_properties(plugin.cpp PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions")
set_source_files_properties(py.cpp PROPERTIES COMPILE_FLAGS "-frtti -fexceptions")

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(APPLE)
  target_link_options(llvm-py-pass PRIVATE
    -Wl,-flat_namespace
    -Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/exports.txt
  )
endif()
