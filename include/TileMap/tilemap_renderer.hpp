#pragma once

#include "Shader/texture.hpp"

#include <GL/glew.h>
#include <cstdint>

struct TileMapData;
class Shader;
class WindowContext;

class TileMapRenderer {
public:
    TileMapRenderer(const TileMapData& data, Shader& shader, const WindowContext& context);
    ~TileMapRenderer();

    // Deleted copy constructors to prevent accidental VRAM multi-allocations
    TileMapRenderer(const TileMapRenderer&) = delete;
    TileMapRenderer& operator=(const TileMapRenderer&) = delete;

    void render(const TileMapData& data, Shader& shader, const WindowContext& context, bool isDirty);

private:
    void initRenderData(const TileMapData& data, Shader& shader, const WindowContext& context);

    Texture m_tileset_tex;
    unsigned int m_quad_vao = 0;
    unsigned int m_quad_vbo = 0;
    unsigned int m_id_vbo   = 0;
};