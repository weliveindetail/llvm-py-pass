cmake_minimum_required(VERSION 3.20)
project(llvm-py-pass)

if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
  message(STATUS "Default enable compile_commands.json")
  set(CMAKE_EXPORT_COMPILE_COMMANDS On)
endif()

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

include(FetchContent)
FetchContent_Declare(pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11
  GIT_TAG        v2.13.6
)

FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
  FetchContent_Populate(pybind11)
  add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()

find_package(LLVM REQUIRED)

add_library(llvm-py-pass SHARED plugin.cpp py.cpp)
target_include_directories(llvm-py-pass PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(llvm-py-pass PRIVATE pybind11::embed LLVMipo)

set_source_files_properties(plugin.cpp PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions")
set_source_files_properties(py.cpp PROPERTIES COMPILE_FLAGS "-frtti -fexceptions")

if(APPLE)
  set(SYSROOT -isysroot ${CMAKE_OSX_SYSROOT})
  target_link_options(llvm-py-pass PRIVATE
    -Wl,-flat_namespace
    -Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/exports.txt
  )
endif()

add_subdirectory(test)

add_custom_target(
    example
    COMMAND ${LLVM_BINARY_DIR}/bin/clang ${SYSROOT} -fpass-plugin=$<TARGET_FILE:llvm-py-pass> -O1 example/test.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running example pass")
