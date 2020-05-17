cmake_minimum_required(VERSION 2.8.3)
project(opengl_cherno)

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenCV REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  roscpp
)
#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)

catkin_package(
    CATKIN_DEPENDS
        roscpp
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)
#include_directories(${GLFW_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(shader_test src/shader_test.cpp)
target_link_libraries(shader_test -lGL -lEGL -lGLEW -lglfw)

add_executable(buffer_test src/index_buffer.cpp)
target_link_libraries(buffer_test -lGL -lEGL -lGLEW -lglfw)

add_executable(uniform src/uniform.cpp)
target_link_libraries(uniform -lGL -lEGL -lGLEW -lglfw)

add_executable(vertex_array src/vertex_array.cpp)
target_link_libraries(vertex_array -lGL -lEGL -lGLEW -lglfw)

add_executable(application
        src/application.cpp
        src/VertexArray.cpp
        src/IndexBuffer.cpp
        src/VertexBuffer.cpp
        src/Shader.cpp
        src/Renderer.cpp
        vendor/stb_image/stb_image.h
        vendor/stb_image/stb_image.cpp
        src/Texture.cpp)
target_link_libraries(application -lGL -lEGL -lGLEW -lglfw -glm)