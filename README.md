# c-gbemu

[![build status](https://github.com/krscott/c-gbemu/actions/workflows/c.yml/badge.svg)](https://github.com/krscott/c-gbemu/actions)

A Work-in-Progress Gameboy emulator written in C.

No graphics yet, but the CPU is mostly functional.

## Usage

Run `c-gbemu` with a ROM as an argument:
```
./c-gbemu Tetris.gb
```

Use `-h` option to list command line options.

## Build

Build debug
```
make
```

Build release
```
make BUILD=release
```

Run tests
```
make check
```

Run Dr. Memory analysis. (Requires `drmemory` to be in `PATH`)
```
make drmem
```

## Windows Setup

### Dependencies

**gcc**

e.g. via msys, see [VSCode guide](https://code.visualstudio.com/docs/cpp/config-mingw)

**make**

e.g. via [chocolatey](https://community.chocolatey.org/packages/make)

**SDL**

e.g. via [msys2](https://www.msys2.org/) again:
```
pacman -S \
    mingw64/mingw-w64-x86_64-SDL2 \
    mingw64/mingw-w64-x86_64-SDL2_mixer \
    mingw64/mingw-w64-x86_64-SDL2_image \
    mingw64/mingw-w64-x86_64-SDL2_ttf \
    mingw64/mingw-w64-x86_64-SDL2_net
```

## C Style Settings

These clang-format settings are used for VSCode:
```
"C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: Google, IndentWidth: 4 }"
```
[More info](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)

## Compliance

Run `make check` to run all tests.

| Blargg Test           | Status |
| --------------------- | :----: |
| 01-special            |   ✅    |
| 02-interrupts         |   ❌    |
| 03-op sp,hl           |   ✅    |
| 04-op r,imm           |   ✅    |
| 05-op rp              |   ✅    |
| 06-ld r,r             |   ✅    |
| 07-jr,jp,call,ret,rst |   ✅    |
| 08-misc instrs        |   ✅    |
| 09-op r,r             |   ✅    |
| 10-bit ops            |   ✅    |
| 11-op a,(hl)          |   ✅    |

Test ROMs by [Blargg](https://web.archive.org/web/20160830121433/http://blargg.8bitalley.com/parodius/gb-tests/).