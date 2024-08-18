#include <iostream>
#include <glm/glm.hpp>



inline std::ostream& operator<<(std::ostream& s, glm::vec3 v) {
  s << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return s;
}
inline std::ostream& operator<<(std::ostream& s, glm::vec2 v) {
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}