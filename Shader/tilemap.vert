#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in int iPaletteID; //Per instance

out vec2 UV;

uniform vec2 u_Palette[256];
uniform vec2 u_TextureSize;
uniform vec2 u_TextureTileSize;


uniform vec2 u_TileSize;
uniform int u_TilesPerRow;

// uniform mat4 view;
// uniform mat4 projection;



void main() {
    int mapCol = gl_InstanceID % u_TilesPerRow;
    int mapRow = gl_InstanceID / u_TilesPerRow;

    vec2 tileGridPos = vec2(float(mapCol) * u_TileSize.x, float(mapRow) * u_TileSize.y);
    vec2 worldPosition = tileGridPos + (aPos * u_TileSize);

    gl_Position = vec4(worldPosition, 0.0f, 1.0f);

    vec2 uvTileSize = u_TextureTileSize / u_TextureSize;
    vec2 uvStartAnchor = u_Palette[iPaletteID] / u_TextureSize;
    UV = uvStartAnchor + (aTexCoords * uvTileSize);
}