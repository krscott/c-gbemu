# c-gbemu

Gameboy emulator written in C

## Windows

Install
- MinGW64 gcc (e.g. [VSCode guide](https://code.visualstudio.com/docs/cpp/config-mingw))
- make (e.g. [chocolatey](https://community.chocolatey.org/packages/make))

Add SDL2 lib
- Download [SDL2 devel mingw](https://github.com/libsdl-org/SDL/releases/download/release-2.24.1/SDL2-devel-2.24.1-mingw.zip) archive
- Copy `x86_64-w64-mingw32/lib` folder into project root
- Copy `x86_64-w64-mingw32/bin/SDL2.dll` into project root

Build
```
make
```
