if(NOT LLVM_TOOLS_DIR)
  set(LLVM_TOOLS_DIR ${LLVM_BINARY_DIR})
endif()
if(NOT LLVM_EXTERNAL_LIT OR NOT EXISTS ${LLVM_EXTERNAL_LIT})
  if(EXISTS ${LLVM_TOOLS_DIR}/bin/llvm-lit)
    set(LLVM_EXTERNAL_LIT ${LLVM_TOOLS_DIR}/bin/llvm-lit)
    message(STATUS "Found llvm-lit: ${LLVM_EXTERNAL_LIT}")
  else()
    find_program(LLVM_EXTERNAL_LIT NAMES llvm-lit lit)
  endif()
endif()
if(LLVM_EXTERNAL_LIT)
  message(STATUS "Found llvm-lit: ${LLVM_EXTERNAL_LIT}")
else()
  message(WANRING "Not found: llvm-lit")
endif()

list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
include(AddLLVM)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in"
               "${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py" @ONLY)

# The plugin scans for the run.py script in CWD and all parent directories
add_custom_target(symlink-test-scripts ALL COMMENT "Symlinking test scripts to build-tree")
file(GLOB_RECURSE scripts "${CMAKE_CURRENT_SOURCE_DIR}/*/run.py")

foreach(file IN LISTS scripts)
  file(RELATIVE_PATH relative_path "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
  set(link_target "${CMAKE_CURRENT_BINARY_DIR}/${relative_path}")

  # Ensure the parent directory exists in the build tree
  get_filename_component(link_target_dir "${link_target}" DIRECTORY)
  file(MAKE_DIRECTORY "${link_target_dir}")

  add_custom_command(
      OUTPUT "${link_target}"
      COMMAND ${CMAKE_COMMAND} -E create_symlink "${file}" "${link_target}"
      DEPENDS "${file}"
      COMMENT "Creating symlink for ${file} -> ${link_target}"
  )

  string(REPLACE "/" "_" relative_path "${relative_path}")
  string(REPLACE "\\" "_" relative_path "${relative_path}")

  add_custom_target("symlink_${relative_path}" DEPENDS "${link_target}")
  list(APPEND LLVM_TEST_DEPENDS "symlink_${relative_path}")
endforeach()

set(LLVM_LIT_ARGS -vv)
add_lit_testsuite(check "Running llvm-py-pass tests"
  ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS llvm-py-pass ${LLVM_TEST_DEPENDS}
)
