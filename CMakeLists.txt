cmake_minimum_required(VERSION 3.20)

# Required cmake variables (pass on command line):
#   -DCMAKE_C_COMPILER=<ndk>/bin/clang
#   -DCMAKE_CXX_COMPILER=<ndk>/bin/clang++
#   -DLLVM_DIR=<path to LLVMConfig.cmake>
project(ollvm-plugin)

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} at ${LLVM_DIR}")
message(STATUS "LLVM library dir: ${LLVM_LIBRARY_DIRS}")

set(CMAKE_CXX_STANDARD 17)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

set(OBFUSCATION_SRS
  Obfuscation/Utils.cpp
  Obfuscation/CryptoUtils.cpp
  Obfuscation/ObfuscationOptions.cpp
  Obfuscation/BogusControlFlow.cpp
  Obfuscation/IPObfuscationContext.cpp
  Obfuscation/Flattening.cpp
  Obfuscation/StringEncryption.cpp
  Obfuscation/SplitBasicBlock.cpp
  Obfuscation/Substitution.cpp
  Obfuscation/IndirectBranch.cpp
  Obfuscation/IndirectCall.cpp
  Obfuscation/IndirectGlobalVariable.cpp
  plugin_reg.cpp
)

add_library(Obfuscation SHARED ${OBFUSCATION_SRS})
target_include_directories(Obfuscation PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(Obfuscation PRIVATE ${CMAKE_SOURCE_DIR}/Obfuscation)
set_target_properties(Obfuscation PROPERTIES
  COMPILE_FLAGS "-fno-rtti -fPIC"
)

# Link LLVM static libraries with --exclude-libs ALL.
# This embeds the needed LLVM code (vtables, cl::opt, IRBuilder, etc.)
# into the plugin, but marks ALL static lib symbols as LOCAL/hidden.
# This prevents symbol conflicts with the host clang (which has its own
# statically-linked LLVM). At runtime:
#   - Plugin uses its own (hidden) copies for internal stuff (vtables, cl::opt)
#   - Host clang provides the pass pipeline infrastructure via dlopen global scope
target_link_options(Obfuscation PRIVATE
  -Wl,--exclude-libs,ALL
  -Wl,--unresolved-symbols=ignore-in-object-files
)

# Collect all needed LLVM static libraries
file(GLOB LLVM_STATIC_LIBS "${LLVM_LIBRARY_DIRS}/libLLVM*.a")
target_link_libraries(Obfuscation PRIVATE ${LLVM_STATIC_LIBS} -lpthread -ldl -lz -ltinfo)
