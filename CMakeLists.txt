cmake_minimum_required(VERSION 3.20)
project(ollvm-plugin)

message(STATUS "LLVM_DIR: ${LLVM_DIR}")

set(CMAKE_C_COMPILER ${LLVM_DIR}/bin/clang)
set(CMAKE_CXX_COMPILER ${LLVM_DIR}/bin/clang++)
list(APPEND CMAKE_PREFIX_PATH "${LLVM_DIR}/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

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
