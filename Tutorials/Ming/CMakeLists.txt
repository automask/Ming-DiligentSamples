cmake_minimum_required(VERSION 3.10)

project(Ming CXX)

# --------------------------------------------------------------------
macro(copy_file_to_app name file_path)
    set(file_name "")
    get_filename_component(file_name ${file_path} NAME)
    message(">>> copy file ${file_name}")

    add_custom_command(
        TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file_path} "$<TARGET_FILE_DIR:${name}>/${file_name}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
endmacro()

# --------------------------------------------------------------------
add_sample_app(M_01_Triangle
    IDE_FOLDER
    DiligentSamples/Ming
    SOURCES
    src/M_01_Triangle.cpp
)

add_sample_app(M_02_Texture
    IDE_FOLDER
    DiligentSamples/Ming
    SOURCES
    src/M_02_Texture.cpp

    SHADERS
    assets/02_texture_cube.vsh
    assets/02_texture_cube.psh

    # ASSETS
    # ../Tutorial03_Texturing/assets/DGLogo.png
)

copy_file_to_app(M_02_Texture ../Tutorial03_Texturing/assets/DGLogo.png)

# add_custom_command(
# TARGET M_02_Texture POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_if_different "../Tutorial03_Texturing/assets/DGLogo.png" "$<TARGET_FILE_DIR:M_02_Texture>/DGLogo.png"
# WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
# )
