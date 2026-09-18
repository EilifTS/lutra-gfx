FetchContent_Declare(GTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(GTest)

add_executable(tests tests.cpp)
target_compile_features(tests PRIVATE cxx_std_20)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tests lutra-gfx GTest::gtest)
target_compile_definitions(tests PRIVATE TESTS_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

include(GoogleTest)
gtest_discover_tests(tests)