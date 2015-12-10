cmake_minimum_required(VERSION 2.6)

project(LOOM)

#
# Include LLVM configuration, working around current limitations in the
# LLVM CMake configuration tools by calling llvm-config directly.
#
# Thanks to David Chisnall for pointing this out and providing a code example
# (https://github.com/CompilerTeaching/SimplePass/blob/master/CMakeLists.txt).
#
find_program(LLVM_CONFIG NAMES llvm-config DOC "Path to llvm-config utility")
if (${LLVM_CONFIG} STREQUAL "LLVM_CONFIG-NOTFOUND")
	message(SEND_ERROR "llvm-config not found, please manually set path with -DLLVM_CONFIG")
endif()
message(STATUS "Using llvm-config: ${LLVM_CONFIG}")

exec_program(${LLVM_CONFIG} ARGS --bindir OUTPUT_VARIABLE LLVM_BINARY_DIR)
exec_program(${LLVM_CONFIG} ARGS --cxxflags OUTPUT_VARIABLE LLVM_DEFINITIONS)
exec_program(${LLVM_CONFIG} ARGS --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIRS)
exec_program(${LLVM_CONFIG} ARGS --libdir   OUTPUT_VARIABLE LLVM_LIBRARY_DIRS)
exec_program(${LLVM_CONFIG} ARGS --prefix OUTPUT_VARIABLE LLVM_PREFIX)

set(CMAKE_MODULE_PATH "${LLVM_PREFIX}/share/llvm/cmake")

set(LLVM_RUNTIME_OUTPUT_INTDIR ".")
set(LLVM_LIBRARY_OUTPUT_INTDIR ".")
include(HandleLLVMOptions)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Always build with all warnings.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wstrict-prototypes")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


add_subdirectory(src)
add_subdirectory(test)
