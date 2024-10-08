#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 textCoord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform[512];

void main() {
  gl_Position = projection * view * transform[gl_InstanceID] * vec4(aPos, 1.0);
  TexCoord = textCoord;
}