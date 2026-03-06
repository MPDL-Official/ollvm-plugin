# ollvm-plugin

OLLVM (Obfuscator-LLVM) pass plugin for Android NDK (LLVM 19).

Builds as a shared library (`libObfuscation.so`) that can be loaded by NDK's clang via `-fpass-plugin=`.

## Obfuscation passes

- `bcf` - Bogus Control Flow
- `fla` - Control Flow Flattening
- `sub` - Instruction Substitution
- `sobf` - String Encryption
- `split` - Basic Block Splitting
- `ibr` - Indirect Branch
- `icall` - Indirect Call
- `igv` - Indirect Global Variable

## Build

```bash
NDK_LLVM=$ANDROID_HOME/ndk/<version>/toolchains/llvm/prebuilt/linux-x86_64

mkdir build && cd build
cmake .. -DLLVM_DIR=$NDK_LLVM -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

Output: `build/libObfuscation.so`

## Usage

```bash
clang -fpass-plugin=libObfuscation.so -mllvm -sobf -mllvm -fla ...
```

## Credits

- [ollvm17](https://github.com/DreamSoule/ollvm17) - Original OLLVM17 by DreamSoule
- [ollvm17#38](https://github.com/DreamSoule/ollvm17/issues/38) - LLVM 19 adaptation by funkvps
- [Hikari-LLVM15-Core](https://github.com/61bcdefg/Hikari-LLVM15-Core) - Improved Flattening pass
