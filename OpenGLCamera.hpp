#pragma once
#include "game/Game.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>



struct OpenGLCamera : public Camera {
  glm::mat4 GetViewMatrix();
};
