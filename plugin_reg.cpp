//===- Parsing and selection of pass pipelines ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
///
/// This file provides the implementation of the PassBuilder based on our
/// static pass registry as well as related functionality. It also provides
/// helpers to aid in analyzing, debugging, and testing passes and pass
/// pipelines.
///
//===----------------------------------------------------------------------===//

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"

// 引用 Obfuscation 相关文件
#include "Obfuscation/BogusControlFlow.h"        // 虚假控制流
#include "Obfuscation/Flattening.h"              // 控制流平坦化
#include "Obfuscation/IndirectBranch.h"          // 间接跳转
#include "Obfuscation/IndirectCall.h"            // 间接调用
#include "Obfuscation/IndirectGlobalVariable.h"  // 间接全局变量
#include "Obfuscation/SplitBasicBlock.h"         // 基本块分割
#include "Obfuscation/StringEncryption.h"        // 字符串加密
#include "Obfuscation/Substitution.h"            // 指令替换
#include "Obfuscation/Utils.h"                   // 为了控制函数名混淆开关 (bool obf_function_name_cmd;)

using namespace llvm;

// Passes enabled by default when plugin is loaded.
// Only lightweight passes are on by default to avoid OOM.
static cl::opt<bool> s_obf_split("split", cl::init(false), cl::desc("SplitBasicBlock: split_num=3(init)"));
static cl::opt<bool> s_obf_sobf("sobf", cl::init(true), cl::desc("String Obfuscation"));
static cl::opt<bool> s_obf_fla("fla", cl::init(false), cl::desc("Flattening"));
static cl::opt<bool> s_obf_sub("sub", cl::init(true), cl::desc("Substitution: sub_loop"));
static cl::opt<bool> s_obf_bcf("bcf", cl::init(false), cl::desc("BogusControlFlow (disabled, high memory usage)"));
static cl::opt<bool> s_obf_ibr("ibr", cl::init(false), cl::desc("Indirect Branch"));
static cl::opt<bool> s_obf_igv("igv", cl::init(false), cl::desc("Indirect Global Variable"));
static cl::opt<bool> s_obf_icall("icall", cl::init(true), cl::desc("Indirect Call"));
static cl::opt<bool> s_obf_fn_name_cmd("fncmd", cl::init(false), cl::desc("use function name control obfuscation(_ + command + _ | example: function_fla_bcf_)"));

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    std::cerr << "Obfuscation Plugin Load ..." << '\n';
    return {
        LLVM_PLUGIN_API_VERSION,
        "Obfuscation",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerAnalysisRegistrationCallback([](ModuleAnalysisManager &MAM) {
                MAM.registerPass([&] { return PassInstrumentationAnalysis(); });
            });

            PB.registerPipelineStartEPCallback(
                [](llvm::ModulePassManager &MPM, llvm::OptimizationLevel Level) {
                    outs() << "[obf] run.registerPipelineStartEPCallback\n";
                    obf_function_name_cmd = s_obf_fn_name_cmd;
                    if (obf_function_name_cmd) {
                        outs() << "[obf] enable function name control obfuscation(_ + command + _ | example: function_fla_)\n";
                    }
                    MPM.addPass(StringEncryptionPass(s_obf_sobf));  // 先进行字符串加密 出现字符串加密基本块以后再进行基本块分割和其他混淆 加大解密难度
                    llvm::FunctionPassManager FPM;
                    FPM.addPass(IndirectCallPass(s_obf_icall));     // 间接调用
                    FPM.addPass(SplitBasicBlockPass(s_obf_split));  // 优先进行基本块分割
                    FPM.addPass(FlatteningPass(s_obf_fla));         // 对于控制流平坦化
                    FPM.addPass(SubstitutionPass(s_obf_sub));       // 指令替换
                    FPM.addPass(BogusControlFlowPass(s_obf_bcf));   // 虚假控制流
                    MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
                    MPM.addPass(IndirectBranchPass(s_obf_ibr));          // 间接指令 理论上间接指令应该放在最后
                    MPM.addPass(IndirectGlobalVariablePass(s_obf_igv));  // 间接全局变量
                });
        }};
}
