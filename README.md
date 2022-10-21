# c-gbemu

![build status](https://github.com/krscott/c-gbemu/actions/workflows/c.yml/badge.svg)

A Work-in-Progress Gameboy emulator written in C.

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

## C Style Settings

These clang-format settings are used for VSCode:
```
"C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: Google, IndentWidth: 4 }"
```
[More info](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)

## Test ROMs

Test ROMs by [Blargg](https://web.archive.org/web/20160830121433/http://blargg.8bitalley.com/parodius/gb-tests/).
