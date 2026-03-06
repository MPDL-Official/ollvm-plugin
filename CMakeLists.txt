cmake_minimum_required(VERSION 3.20)

# NDK_LLVM must be passed via -DNDK_LLVM=<path>
# Compilers must be set BEFORE project() via -DCMAKE_C_COMPILER / -DCMAKE_CXX_COMPILER
project(ollvm-plugin)

message(STATUS "NDK_LLVM: ${NDK_LLVM}")

set(LLVM_DIR "${NDK_LLVM}/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} at ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 17)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

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

set(CMAKE_SKIP_BUILD_RPATH ON)
add_library(Obfuscation SHARED ${OBFUSCATION_SRS})
target_include_directories(Obfuscation PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(Obfuscation PRIVATE ${CMAKE_SOURCE_DIR}/Obfuscation)
set_target_properties(Obfuscation PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)
target_link_libraries(Obfuscation PRIVATE LLVM)
