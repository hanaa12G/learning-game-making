#include "Camera.hpp"
#include <cstdio>

Camera::Camera() {
  world_up = glm::vec3 (0.0f, 1.0f, 0.0f);
  up = glm::vec3 (0.0f, 1.0f, 0.0);
  right = glm::vec3 (1.0f, 0.0f, 0.0f);
  location = glm::vec3 (0.0f, 0.0f, 0.0f);
  direction = glm::vec3 (0.0f, 0.0f, 1.0f);

  speed = 0.1;
  sensitivity = 0.1;
  yaw = 90;
  pitch = 0;
}

void Camera::Move(glm::vec3 d) {
  location += glm::vec3(d.x, d.y, d.z);
  UpdateVector();
}

void Camera::Rotate(float y, float p) {
  yaw -= y * sensitivity;
  pitch += p * sensitivity;

  glm::vec3 direction_change;
  direction_change.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  direction_change.y = std::sin(glm::radians(pitch));
  direction_change.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

  fprintf(stdout, "%f %f %f\n", direction_change.x, direction_change.y, direction_change.z);

  direction = direction_change;
  UpdateVector();
}



void Camera::UpdateVector() {
  direction = glm::normalize(direction);

  // direction changes, update right
  right = glm::cross(direction, world_up);
  right = glm::normalize(right);

  // right changes, update up
  up = glm::cross(right, direction);
  up = glm::normalize(up);
}