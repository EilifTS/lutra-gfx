cmake_minimum_required (VERSION 3.28)
project(efvk LANGUAGES CXX)

add_subdirectory ("src")

include(FetchContent)
FetchContent_Declare(glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(glfw3)

find_package(Vulkan REQUIRED)

add_executable(main src/main.cpp "src/Event.h" "src/InputManager.cpp" "src/InputManager.h" "src/Keyboard.cpp" "src/Keyboard.h" "src/Mouse.cpp" "src/Mouse.h" "src/Window.cpp" "src/Window.h" "src/Utils.h" "src/GraphicsContext.h" "src/GraphicsContext.cpp")
target_link_libraries(main PRIVATE glfw Vulkan::Vulkan)
target_include_directories(main PRIVATE src)
target_compile_features(main PRIVATE cxx_std_20)
