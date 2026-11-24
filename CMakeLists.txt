cmake_minimum_required(VERSION 4.2)

project(agent_c
    VERSION 0.1.0
    DESCRIPTION "A minimal C11 application"
    LANGUAGES C)

# Strict C11: no compiler extensions, no silent fallback.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Handy for editors and clangd; the resulting file is gitignored.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Single-config generators have no build type by default, which means no
# optimization and no debug info. Pick a sensible default instead.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# One executable; all sources and private headers live under src/.
add_executable(agent
    src/main.c
    src/history.c
    src/session.c
    src/term.c
    src/msg.c
    src/user.c
    src/shell.c
)
target_include_directories(agent PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Add cJSON
add_library(cjson STATIC external/cJSON/cJSON.c)
target_include_directories(cjson PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/cJSON")
target_link_libraries(agent PRIVATE cjson)

# Add libcURL
find_package(CURL REQUIRED)
target_link_libraries(agent PRIVATE CURL::libcurl)

if(MSVC)
    target_compile_options(agent PRIVATE /W4)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(agent PRIVATE -Wall -Wextra -Wpedantic)
endif()
