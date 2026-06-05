#version 330 core
in vec2 UV;
in vec4 TintColor;
out vec4 FragColor;

uniform sampler2D u_TextureAtlas;

void main() {
    FragColor = texture(u_TextureAtlas, UV) * TintColor;
}