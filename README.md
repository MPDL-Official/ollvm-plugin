# ollvm-clang

Build a custom LLVM/Clang with OLLVM obfuscation passes baked in.

## What this does

Downloads LLVM 19.1.7 source, patches it with OLLVM obfuscation passes, and builds a custom `clang` binary. The resulting clang supports `-mllvm -sobf`, `-mllvm -fla`, etc. natively.

## Obfuscation passes

| Flag | Description |
|------|-------------|
| `-mllvm -sobf` | String Encryption |
| `-mllvm -fla` | Control Flow Flattening |
| `-mllvm -sub` | Instruction Substitution |
| `-mllvm -bcf` | Bogus Control Flow |
| `-mllvm -split` | Basic Block Splitting |
| `-mllvm -icall` | Indirect Call |
| `-mllvm -ibr` | Indirect Branch |
| `-mllvm -igv` | Indirect Global Variable |

## Usage with Android NDK

Replace NDK's clang with the custom-built one:

```bash
NDK_LLVM=$ANDROID_HOME/ndk/28.2.13676358/toolchains/llvm/prebuilt/linux-x86_64
# Backup original
cp $NDK_LLVM/bin/clang-19 $NDK_LLVM/bin/clang-19.bak
# Replace with OLLVM clang
cp ollvm-clang/clang-19 $NDK_LLVM/bin/clang-19
```

Then use standard NDK build with `-mllvm` flags.

## Credits

- [ollvm17](https://github.com/DreamSoule/ollvm17) - Original OLLVM17 by DreamSoule
- [ollvm17#38](https://github.com/DreamSoule/ollvm17/issues/38) - LLVM 19 adaptation by funkvps
- [Hikari-LLVM15-Core](https://github.com/61bcdefg/Hikari-LLVM15-Core) - Improved Flattening pass
