#include "OpenGLCamera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>


glm::mat4 OpenGLCamera::GetViewMatrix() {
  return glm::lookAt(location, location + direction, up);
}
