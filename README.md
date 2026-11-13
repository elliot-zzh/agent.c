# agent.c

A minimal C11 application built with CMake.

## Requirements

- CMake 3.16 or newer (3.21+ for the presets below, 4.2+ for the Windows preset)
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

## Presets

`CMakePresets.json` needs CMake 3.21 or newer. Presets are gated by host OS and
put their build trees in `build/<preset-name>/`.

```sh
# macOS (Unix Makefiles, system compiler)
cmake --preset macos
cmake --build --preset macos
./build/macos/agent

cmake --preset macos-release
cmake --build --preset macos-release
```

The `windows` preset uses the `Visual Studio 18 2026` generator (x64, default
`v145` toolset) with the vcpkg toolchain. That generator arrived in CMake 4.2.
Set `VCPKG_ROOT` to your vcpkg checkout first:

```powershell
$env:VCPKG_ROOT = "C:\vcpkg"
cmake --preset windows
cmake --build --preset windows          # Debug
cmake --build --preset windows-release  # Release
```

Visual Studio is multi-config, so one configure covers both Debug and Release;
only the build preset selects the configuration. Machine-specific settings
(such as a hardcoded vcpkg path or a Ninja generator) belong in
`CMakeUserPresets.json`, which is gitignored.
