#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 viColor;

out vec3 vColor;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * transform * vec4(vPos, 1.0f);
  vColor = viColor;
}
