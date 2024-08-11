cmake_minimum_required(VERSION 3.20)

set(SOURCE
    Game.hpp
    Objects.hpp
    Collision.hpp
    Camera.hpp
)

add_library(game2
    ${SOURCE}
)

target_link_libraries(game2 glm::glm)