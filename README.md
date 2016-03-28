LOOM: LLVM instrumentation library
====================================================================

LOOM is a general-purpose library for adding instrumentation to software in
the LLVM intermediate representation (IR) format. It is currently capable of
generating static instrumentation points that expose values (e.g., function
parameters and return values) to software-defined instrumentation functions.

LOOM is part of the
[CADETS research project](https://www.cl.cam.ac.uk/research/security/cadets)
(Causal, Adaptive, Distributed, and Efficient Tracing System).

BUILDING INSTRUCTIONS
=====================================================================

1. Download llvm from github.
git clone https://github.com/cadets/llvm llvmLoom

2. Checkout the loom branch of llvm.
cd llvmLoom && git checkout remotes/origin/loom

3. Run cmake in the build directory created in the llvm directory (llvmLoom).
mkdir build && cd build && cmake -D LLVM_ENABLE_MODULES=ON -G Ninja ..

4. Build it with ninja.
ninja

5. Set your environment variable PATH. One method for unix-like systems is adding the following line to the .profile file in your home directory. (If there is no .profile, create one.)
PATH='PATH_TO_LLVM_DIRECTORY'/build/bin:$PATH

6. Go up two directories.
cd ../..

7. Download loom.
git clone https://github.com/cadets/loom loom


8. Change 'src' to 'lib' in the following line in file 'PATH_TO_LOOM'/test/lit.cfg. 
[ os.path.join(loom_build, 'src') ])

9. Add the following lines before the line 'include(AddLLVM)' in the file 'loom/CMakeLists.txt'
# Retrieved from 'http://stackoverflow.com/questions/27863706/llvm-out-of-source-pass-build-loadable-modules-not-supported-on-linux'
# AddLLVM needs these in loom/CMakeLists.txt  
# llvmLoom/build/share/llvm/cmake/AddLLVM.cmake
set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib)
include(HandleLLVMOptions) # important: matches compiler flags to LLVM/Clang build

10. cd loom && mkdir build && cd build

11. Run cmake.
cmake -GNinja ..

12. Build it with ninja.
ninja 
