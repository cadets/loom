cmake_minimum_required(VERSION 2.6)

project(LOOM)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

# Based on http://stackoverflow.com/questions/27863706:
#
# AddLLVM expects LLVM{LIBRARY,RUNTIME}_OUTPUT_INTDIR to be set in order
# to build successfully out of the LLVM tree on non-Mac platforms.
set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib)

include(HandleLLVMOptions)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Where to find LLVM binaries (e.g., llvm-lit).
set(LLVM_BINARY_DIR ${LLVM_DIR}/../../../bin)

# Always use C++11.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Always build with all warnings.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wstrict-prototypes")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_subdirectory(src)
add_subdirectory(test)
