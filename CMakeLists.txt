cmake_minimum_required (VERSION 3.28)
project(efvk LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(glfw3)

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

find_package(Vulkan REQUIRED)

file(GLOB EFVK_INCLUDES
    include/efvk/*.h
    include/efvk/math/*.h
)

file(GLOB EFVK_SOURCES
    src/efvk/*.h
    src/efvk/*.cpp
)

add_executable(main src/main.cpp ${EFVK_INCLUDES} ${EFVK_SOURCES})
target_link_libraries(main PRIVATE glfw spng_static ZLIB::ZLIB Vulkan::Vulkan)
target_include_directories(main PRIVATE "include")
target_compile_features(main PRIVATE cxx_std_20)

# Copy shaders
add_custom_target(copy_shaders)

add_custom_command(
    TARGET copy_shaders PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

set(SHADER_SRC_DIR "${CMAKE_SOURCE_DIR}/src/efvk/shaders")

file(GLOB SPIRV_SHADERS
   ${SHADER_SRC_DIR}/*.spv
)

foreach(SPV_FILE_PATH IN LISTS SPIRV_SHADERS)
file(RELATIVE_PATH SPV_FILE_NAME ${SHADER_SRC_DIR} ${SPV_FILE_PATH})
add_custom_command(
    TARGET copy_shaders PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SPV_FILE_PATH}
            ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SPV_FILE_NAME}
)
endforeach()
add_dependencies(main copy_shaders)

# Copy textures
add_custom_target(copy_textures)

add_custom_command(
    TARGET copy_textures PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/textures
)

set(TEXTURE_SRC_DIR "${CMAKE_SOURCE_DIR}/src/efvk/textures")

file(GLOB TEXTURES
   ${TEXTURE_SRC_DIR}/*.png
)

foreach(TEXTURE_FILE_PATH IN LISTS TEXTURES)
file(RELATIVE_PATH TEXTURE_FILE_NAME ${TEXTURE_SRC_DIR} ${TEXTURE_FILE_PATH})
add_custom_command(
    TARGET copy_textures PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${TEXTURE_FILE_PATH}
            ${CMAKE_CURRENT_BINARY_DIR}/textures/${TEXTURE_FILE_NAME}
)
endforeach()
add_dependencies(main copy_textures)