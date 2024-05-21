#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


enum class CameraMovement {
  Left,
  Right,
  Forward,
  Backward,
  FlightUp,
  FlightDown
};

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


  Camera();
  void Move(CameraMovement mm, int unit);
  void Rotate(int yaw, int pitch);

  glm::mat4 GetViewMatrix();
};
