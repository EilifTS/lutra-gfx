cmake_minimum_required (VERSION 3.28)
project(lutra-gfx LANGUAGES CXX)

option(LUTRA_GFX_ENABLE_IMGUI "Enable ImGui" ON)
option(LUTRA_GFX_BUILD_SAMPLES "Build samples" ON)
option(LUTRA_GFX_BUILD_TESTS "Build tests" ON)

# Define library files
file(GLOB LUTRA_GFX_INCLUDES CONFIGURE_DEPENDS
    include/lutra-gfx/*.h
)

file(GLOB LUTRA_GFX_SOURCES CONFIGURE_DEPENDS
    src/lutra-gfx/*.h
    src/lutra-gfx/*.cpp
    src/lutra-gfx/internal/*.h
    src/lutra-gfx/internal/*.cpp
)

# Create library
add_library(lutra-gfx STATIC ${LUTRA_GFX_INCLUDES} ${LUTRA_GFX_SOURCES})

include(FetchContent)

# Vulkan Headers
FetchContent_Declare(
    Vulkan-Headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG vulkan-sdk-1.4.335.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(Vulkan-Headers)
target_link_libraries(lutra-gfx PRIVATE Vulkan-Headers)

# Volk
FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG vulkan-sdk-1.4.335.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(volk)
target_link_libraries(lutra-gfx PRIVATE volk_headers)

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
if(NOT DEFINED CACHE{ZLIB_COMPAT})
    set(ZLIB_COMPAT ON CACHE BOOL "")
endif()
if(NOT DEFINED CACHE{ZLIB_ENABLE_TESTS})
    set(ZLIB_ENABLE_TESTS OFF CACHE BOOL "")
endif()
FetchContent_MakeAvailable(zlib-ng)
set(ZLIB_INCLUDE_DIR "${CMAKE_BINARY_DIR}/zlib-ng" CACHE PATH "Path to zlib include directory")
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
if(NOT DEFINED CACHE{BUILD_EXAMPLES})
    set(BUILD_EXAMPLES OFF CACHE BOOL "")
endif()
if(NOT DEFINED CACHE{SPNG_STATIC})
    set(SPNG_STATIC ON CACHE BOOL "")
endif()
if(NOT DEFINED CACHE{SPNG_SHARED})
    set(SPNG_SHARED OFF CACHE BOOL "")
endif()
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
        src/lutra-gfx/internal/VolkUsage.cpp
    )

    add_library(imgui STATIC ${IMGUI_SRC})
    target_include_directories(
        imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    target_compile_features(imgui PRIVATE cxx_std_20)
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_VULKAN_USE_VOLK)
    target_link_libraries(imgui PRIVATE Vulkan-Headers volk_headers glfw)

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
    enable_testing()
    add_subdirectory(tests)
endif()
