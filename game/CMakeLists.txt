cmake_minimum_required(VERSION 3.20)

set(SOURCE
    Game.cpp
    Objects.cpp
    Collision.cpp
    Camera.cpp
    RandomGenerator.cpp
)

add_library(game2
    ${SOURCE}
)

target_link_libraries(game2 glm::glm)