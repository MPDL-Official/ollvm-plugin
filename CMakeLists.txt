cmake_minimum_required(VERSION 3.20)

# Required cmake variables (pass on command line):
#   -DCMAKE_C_COMPILER=<ndk>/bin/clang
#   -DCMAKE_CXX_COMPILER=<ndk>/bin/clang++
#   -DNDK_LLVM=<ndk toolchain root>
project(ollvm-plugin)

if(NOT NDK_LLVM)
  message(FATAL_ERROR "NDK_LLVM must be set to the NDK toolchain path")
endif()

# NDK LLVM paths - no find_package needed
set(LLVM_INCLUDE_DIR "${NDK_LLVM}/include")
set(LLVM_LIB_DIR "${NDK_LLVM}/lib")

if(NOT EXISTS "${LLVM_INCLUDE_DIR}/llvm/Pass.h")
  message(FATAL_ERROR "LLVM headers not found at ${LLVM_INCLUDE_DIR}")
endif()

message(STATUS "NDK LLVM include: ${LLVM_INCLUDE_DIR}")
message(STATUS "NDK LLVM lib: ${LLVM_LIB_DIR}")

set(CMAKE_CXX_STANDARD 17)

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

target_include_directories(Obfuscation PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/Obfuscation
  ${LLVM_INCLUDE_DIR}
)

target_link_directories(Obfuscation PRIVATE ${LLVM_LIB_DIR})

set_target_properties(Obfuscation PROPERTIES
  COMPILE_FLAGS "-fno-rtti -fPIC"
)

# Link against libLLVM shared library if available, otherwise no link needed
# (pass plugins typically don't link LLVM - symbols come from the host clang)
if(EXISTS "${LLVM_LIB_DIR}/libLLVM.so")
  target_link_libraries(Obfuscation PRIVATE LLVM)
else()
  message(STATUS "No libLLVM.so found - building without LLVM linkage (symbols from host clang)")
endif()
