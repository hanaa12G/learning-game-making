#version 330 core
out vec4 FragColor;

in vec3 outColor;
in vec2 TexCoord;

uniform sampler2D iTexture;

void main() {
  FragColor = vec4(outColor, 1.0) * texture(iTexture, TexCoord);
}