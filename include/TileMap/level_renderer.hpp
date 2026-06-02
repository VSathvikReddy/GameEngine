#pragma once

#include "TileMap/level_data.hpp"
#include "Shader/texture.hpp"
#include "Shader/shader.hpp"
#include "System/system.hpp"

#include <cstdint>

class LevelRenderer {
public:
    // Constructor now takes the shader program input
    LevelRenderer(const LevelData& data, Shader& shader);
    ~LevelRenderer();

    void render(LevelData& data, bool isDirty, Shader& shader, const WindowContext& context);

private:
    uint32_t m_quad_vao = 0;
    uint32_t m_quad_vbo = 0;
    uint32_t m_id_vbo = 0;

    Texture m_tileset_tex;

    // initRenderData accepts the shader program reference
    void initRenderData(const LevelData& data, Shader& shader);
};