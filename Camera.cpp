#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

void Camera::Move(CameraMovement mm, int unit) {
  float distance = unit * speed;
  switch (mm) {
    case CameraMovement::Left:
      location += right * -distance;
      break;
    case CameraMovement::Right:
      location += right * distance;
      break;
    case CameraMovement::Forward:
      location += direction * distance;
      break;
    case CameraMovement::Backward:
      location += direction * -distance;
      break;
    case CameraMovement::FlightUp:
      location += up * distance;
      break;
    case CameraMovement::FlightDown:
      location += up * -distance;
      break;
  }
}

void Camera::Rotate(int unit_yaw, int unit_pitch) {
  float degree_yaw = unit_yaw * sensitivity;
  float degree_pitch = unit_pitch * sensitivity;

  yaw -= degree_yaw;
  pitch += degree_pitch;

  glm::vec3 direction_change;
  direction_change.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  direction_change.y = std::sin(glm::radians(pitch));
  direction_change.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

  fprintf(stdout, "%f %f %f\n", direction_change.x, direction_change.y, direction_change.z);

  direction = direction_change;
  direction = glm::normalize(direction);

  // direction changes, update right
  right = glm::cross(direction, world_up);
  right = glm::normalize(right);

  // right changes, update up
  up = glm::cross(right, direction);
  up = glm::normalize(up);
}

glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(location, location + direction, up);
}
