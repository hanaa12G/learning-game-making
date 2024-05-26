cmake_minimum_required(VERSION 3.20)

project (game)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Stb REQUIRED)
find_package(glm REQUIRED)

add_executable(game main.cpp ShaderProgram.cpp OpenGLCamera.cpp game/Game.cpp game/Camera.cpp)
target_include_directories(game PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(game SDL2::SDL2 OpenGL::GL GLEW::GLEW glm::glm)
