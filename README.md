# agent.c

A minimal C11 application built with CMake.

## Requirements

- CMake 3.16 or newer
- A C11 compiler (GCC, Clang, AppleClang, MSVC, ...)

## Layout

```
CMakeLists.txt   build settings and the single executable target
src/main.c       entry point
src/agent.c      application code
src/agent.h      private header shared between the two translation units
```

All sources and private headers live in `src/`. There is no public `include/`
directory because nothing outside this application consumes the headers, and
no library target because there are no external consumers to link against.

## Build and run

```sh
cmake -S . -B build
cmake --build build
./build/agent
./build/agent Ada
```

Pass `-DCMAKE_BUILD_TYPE=Release` to configure for an optimized build. The
default is `Debug` when the generator does not supply a build type.
`compile_commands.json` is generated in `build/` for editor tooling.
