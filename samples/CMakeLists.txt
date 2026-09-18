function(lutra_gfx_copy_sample_assets target_name)
    set(shader_target ${target_name}_copy_shaders)
    add_custom_target(${shader_target})

    add_custom_command(
        TARGET ${shader_target} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/shaders
    )

    set(SHADER_SRC_DIR "${CMAKE_SOURCE_DIR}/samples/shaders")

    file(GLOB SPIRV_SHADERS CONFIGURE_DEPENDS
       ${SHADER_SRC_DIR}/*.spv
    )

    foreach(SPV_FILE_PATH IN LISTS SPIRV_SHADERS)
    file(RELATIVE_PATH SPV_FILE_NAME ${SHADER_SRC_DIR} ${SPV_FILE_PATH})
    add_custom_command(
        TARGET ${shader_target} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SPV_FILE_PATH}
                ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SPV_FILE_NAME}
    )
    endforeach()
    add_dependencies(${target_name} ${shader_target})

    set(texture_target ${target_name}_copy_textures)
    add_custom_target(${texture_target})

    add_custom_command(
        TARGET ${texture_target} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/textures
    )

    set(TEXTURE_SRC_DIR "${CMAKE_SOURCE_DIR}/samples/textures")

    file(GLOB TEXTURES CONFIGURE_DEPENDS
       ${TEXTURE_SRC_DIR}/*.png
    )

    foreach(TEXTURE_FILE_PATH IN LISTS TEXTURES)
    file(RELATIVE_PATH TEXTURE_FILE_NAME ${TEXTURE_SRC_DIR} ${TEXTURE_FILE_PATH})
    add_custom_command(
        TARGET ${texture_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${TEXTURE_FILE_PATH}
                ${CMAKE_CURRENT_BINARY_DIR}/textures/${TEXTURE_FILE_NAME}
    )
    endforeach()
    add_dependencies(${target_name} ${texture_target})
endfunction()

file(GLOB SAMPLE1_SOURCES CONFIGURE_DEPENDS
    SpriteBatch.cpp
    SpriteBatch.h
    sample1.cpp
)

add_executable(sample1 ${SAMPLE1_SOURCES})
target_compile_features(sample1 PRIVATE cxx_std_20)
target_include_directories(sample1 PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(sample1 lutra-gfx)

lutra_gfx_copy_sample_assets(sample1)
