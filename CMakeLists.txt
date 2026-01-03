cmake_minimum_required (VERSION 3.28)
project(lutra-gfx LANGUAGES CXX)

option(LUTRA_GFX_ENABLE_IMGUI "Enable ImGui" ON)
option(LUTRA_GFX_BUILD_SAMPLES "Build samples" ON)
option(LUTRA_GFX_BUILD_TESTS "Build tests" ON)

# Define library files
file(GLOB LUTRA_GFX_INCLUDES
    include/lutra-vk/*.h
)

file(GLOB LUTRA_GFX_SOURCES
    src/lutra-gfx/*.h
    src/lutra-gfx/*.cpp
    src/lutra-gfx/internal/*.h
    src/lutra-gfx/internal/*.cpp
)

# Create library
add_library(lutra-gfx STATIC ${LUTRA_GFX_INCLUDES} ${LUTRA_GFX_SOURCES})

# Find Vulkan SDK
find_package(Vulkan REQUIRED)
target_link_libraries(lutra-gfx PRIVATE Vulkan::Vulkan)

include(FetchContent)

# VMA
FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG v3.3.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(vma)
target_link_libraries(lutra-gfx PRIVATE GPUOpen::VulkanMemoryAllocator)

# GLFW3
FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(glfw3)
target_link_libraries(lutra-gfx PRIVATE glfw)

# ZLIB-NG
FetchContent_Declare(
    zlib-ng
    GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
    GIT_TAG 2.2.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
set(ZLIB_COMPAT ON CACHE BOOL "" FORCE)
set(ZLIB_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zlib-ng)
set(ZLIB_INCLUDE_DIR "${CMAKE_BINARY_DIR}/zlib-ng" CACHE BOOL "Path to zlib include directory")
set(ZLIB_LIBRARY ZLIB::ZLIB CACHE STRING "Path to zlib library")
add_library(ZLIB::ZLIB ALIAS zlib)
target_link_libraries(lutra-gfx PRIVATE ZLIB::ZLIB)

# SPNG
FetchContent_Declare(
    spng
    GIT_REPOSITORY https://github.com/randy408/libspng.git
    GIT_TAG v0.7.4
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPNG_STATIC ON CACHE BOOL "" FORCE)
set(SPNG_SHARED OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spng)
target_link_libraries(lutra-gfx PRIVATE spng_static)

# ImGUI
if (LUTRA_GFX_ENABLE_IMGUI)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(imgui)

    set(IMGUI_SRC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )

    add_library(imgui STATIC ${IMGUI_SRC})
    target_include_directories(
        imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)
    target_compile_features(imgui PRIVATE cxx_std_20)

    target_compile_definitions(lutra-gfx PUBLIC USE_IMGUI)
    target_link_libraries(lutra-gfx PUBLIC imgui)
endif()

# Finish up library
target_include_directories(lutra-gfx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(lutra-gfx PRIVATE cxx_std_20)

# Detect MACOS and enable Vulkan portability
if(APPLE)
target_compile_definitions(lutra-gfx PRIVATE ENABLE_PORTABILITY=1)
endif()

# Samples
if (LUTRA_GFX_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

# Tests
if (LUTRA_GFX_BUILD_TESTS)
    add_subdirectory(tests)
endif()
