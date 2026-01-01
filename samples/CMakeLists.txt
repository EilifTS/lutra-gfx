file(GLOB SAMPLE1_SOURCES
    SpriteBatch.cpp
    SpriteBatch.h
    sample1.cpp
)

add_executable(sample1 ${SAMPLE1_SOURCES})
target_compile_features(sample1 PRIVATE cxx_std_20)
target_include_directories(sample1 PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(sample1 efvk)

# Copy shaders
add_custom_target(copy_shaders)

add_custom_command(
    TARGET copy_shaders PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

set(SHADER_SRC_DIR "${CMAKE_SOURCE_DIR}/samples/shaders")

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
add_dependencies(sample1 copy_shaders)

# Copy textures
add_custom_target(copy_textures)

add_custom_command(
    TARGET copy_textures PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/textures
)

set(TEXTURE_SRC_DIR "${CMAKE_SOURCE_DIR}/samples/textures")

file(GLOB TEXTURES
   ${TEXTURE_SRC_DIR}/*.png
)

foreach(TEXTURE_FILE_PATH IN LISTS TEXTURES)
file(RELATIVE_PATH TEXTURE_FILE_NAME ${TEXTURE_SRC_DIR} ${TEXTURE_FILE_PATH})
add_custom_command(
    TARGET copy_textures POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${TEXTURE_FILE_PATH}
            ${CMAKE_CURRENT_BINARY_DIR}/textures/${TEXTURE_FILE_NAME}
)
endforeach()
add_dependencies(sample1 copy_textures)
