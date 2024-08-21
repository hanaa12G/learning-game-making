cmake_minimum_required(VERSION 3.20)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O0 -fno-omit-frame-pointer -fno-inline-small-functions")

project (game)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Stb REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(game)

# add_library(game game/Game.cpp game/Camera.cpp)
# target_link_libraries(game glm::glm)
# target_compile_options(game PRIVATE -Wfatal-errors)

add_executable(platform main.cpp ShaderProgram.cpp OpenGLCamera.cpp)
target_include_directories(platform PRIVATE ${Stb_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/game )
target_link_libraries(platform game2 SDL2::SDL2 OpenGL::GL GLEW::GLEW glm::glm)


# enable_testing()
# find_package(Catch2 3 REQUIRED)
# add_executable(tests test/game/test_collision.cpp)
# target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR})
# target_link_libraries(tests PRIVATE Catch2::Catch2WithMain game)