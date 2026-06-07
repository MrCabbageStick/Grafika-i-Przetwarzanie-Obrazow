cmake_minimum_required(VERSION 3.16)
project(gl_project)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(OpenGL REQUIRED)

qt_add_executable(gl_project main.cpp geometry.cpp glslprogram.cpp glwidget.cpp primitives.cpp
                    geometry.h glslprogram.h glwidget.h primitives.h shaders/vs.glsl shaders/fs.glsl
                    utils.h
                    shaders/ads_fs.glsl shaders/ads_vs.glsl)

target_link_libraries(gl_project PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    OpenGL::GL
)

target_include_directories(gl_project PRIVATE .)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            ${CMAKE_BINARY_DIR}/shaders
)

add_dependencies(gl_project copy_shaders)
