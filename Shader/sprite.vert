#version 330 core
layout (location = 0) in vec2 aPos;         // Fixed quad vertex (0,0) to (1,1)
layout (location = 1) in vec2 aTexCoords;

// 32-Byte Instanced Stream Attributes
layout (location = 2) in vec2 iPosition;
layout (location = 3) in vec2 iRotatedRight;
layout (location = 4) in vec2 iRotatedUp;
layout (location = 5) in uint iPaletteID;
layout (location = 6) in vec4 iColor;       // Normalized from 4 GL_UNSIGNED_BYTEs!

out vec2 UV;
out vec4 TintColor;

layout (std140) uniform Camera{
    uniform mat4 view;
    uniform mat4 projection;
};

uniform vec2 u_TextureSize;

layout (std140) uniform PaletteBlock {
    vec4 u_Palette[256];
};

void main() {
    vec2 finalWorldPos = iPosition + (aPos.x * iRotatedRight) + (aPos.y * iRotatedUp);
    gl_Position = projection * view * vec4(finalWorldPos, 0.0f, 1.0f);

    UV = (u_Palette[iPaletteID].xy + (aTexCoords * u_Palette[iPaletteID].zw)) / u_TextureSize;
    TintColor = iColor;
}