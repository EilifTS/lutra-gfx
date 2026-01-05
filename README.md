# LutraGFX

## Purpose
LutraGFX is a thin layer around Vulkan and GLFW with the goal of being easy to use.

## CMake support
LutraGFX can easily be integrated as a static library using cmake.

```cmake

include(FetchContent)
FetchContent_Declare(lutra-gfx
    GIT_REPOSITORY https://github.com/EilifTS/lutra-gfx.git
    GIT_TAG <Some tag>
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
set(LUTRA_GFX_USE_IMGUI ON CACHE BOOL "" FORCE)
set(LUTRA_GFX_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LUTRA_GFX_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(lutra-gfx)

```