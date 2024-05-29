#pragma once

#include <glm/glm.hpp>

struct Camera {
  glm::vec3 location;
  glm::vec3 world_up;
  glm::vec3 right;
  glm::vec3 direction;
  glm::vec3 up;

  float speed;
  float sensitivity;
  float yaw;
  float pitch;

  virtual ~Camera() {}
  Camera();
  virtual void Move(glm::vec3 d);
  virtual void Rotate(float, float);
  virtual void UpdateVector();
};
