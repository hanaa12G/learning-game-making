#version 330 core
out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D iTexture;

void main() {
  FragColor = texture(iTexture, oTexCoord);
}