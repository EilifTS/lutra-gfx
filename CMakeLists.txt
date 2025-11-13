cmake_minimum_required (VERSION 3.28)
project(efvk LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(glfw3)

find_package(Vulkan REQUIRED)

file(GLOB EFVK_INCLUDES
    include/efvk/*.h
)

file(GLOB EFVK_SOURCES
    src/efvk/*.h
    src/efvk/*.cpp
)

add_executable(main src/main.cpp ${EFVK_INCLUDES} ${EFVK_SOURCES})
target_link_libraries(main PRIVATE glfw Vulkan::Vulkan)
target_include_directories(main PRIVATE "include")
target_compile_features(main PRIVATE cxx_std_20)
