#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 textCoord;
layout (location = 2) in vec4 aColor;

out vec2 TexCoord;
out vec4 Color;

uniform mat4 transform;
uniform mat4 projection;


void main() {
  gl_Position = projection * transform * vec4(aPos, 1.0);
  TexCoord = textCoord;
  Color = aColor;
}