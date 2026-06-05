
#include "TileMap/tilemap_renderer.hpp"

#include "TileMap/tilemap_data.hpp"


#include "Shader/shader.hpp"
#include "Render/Texture/texture.hpp"
#include "Render/Texture/texture_manager.hpp"
#include "System/system.hpp"
#include "Shader/uniform.hpp"


#include <GL/glew.h>
#include <cstdint>


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

TileMapRenderer::TileMapRenderer(const TileMapData& data, Shader& shader,const WindowContext& context) 
    : m_tileset_tex(Texture::load(data.texture_file.c_str())) {
    initRenderData(data, shader, context);
}

TileMapRenderer::~TileMapRenderer() {
    if (m_quad_vao != 0) glDeleteVertexArrays(1, &m_quad_vao);
    if (m_quad_vbo != 0) glDeleteBuffers(1, &m_quad_vbo);
    if (m_id_vbo   != 0) glDeleteBuffers(1, &m_id_vbo);
}

void TileMapRenderer::initRenderData(const TileMapData& data, Shader& shader, const WindowContext& context) {
    // Clean, intuitive Top-Left layout for a 2D quad geometry block
    float vertices[] = { 
        // Position (X, Y)  // Texture Coordinates (U, V)
        0.0f, 1.0f,         0.0f, 1.0f, // Bottom-Left
        1.0f, 0.0f,         1.0f, 0.0f, // Top-Right
        0.0f, 0.0f,         0.0f, 0.0f, // Top-Left 

        0.0f, 1.0f,         0.0f, 1.0f, // Bottom-Left
        1.0f, 1.0f,         1.0f, 1.0f, // Bottom-Right
        1.0f, 0.0f,         1.0f, 0.0f  // Top-Right
    };

    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);
    glGenBuffers(1, &m_id_vbo);

    glBindVertexArray(m_quad_vao);

    // STATIC QUAD GEOMETRY PIPELINE
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 0: Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // Attribute 1: Texture Coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // DYNAMIC TILE INSTANCE IDs PIPELINE
    glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
    glBufferData(GL_ARRAY_BUFFER, data.map_grid.size() * sizeof(uint32_t), data.map_grid.data(), GL_DYNAMIC_DRAW);
    
    // Attribute 2: Palette Index (Must use 'I' Pointer variant for pure unsigned integers!)
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(uint32_t), (void*)0);
    glVertexAttribDivisor(2, 1); 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ==========================================
    // INITIALIZATION UNIFORM UPLOAD (RUNS ONCE)
    // ==========================================
    shader.use();

    int atlas_tiles_per_row = m_tileset_tex.getWidth() / static_cast<int>(data.texture_tile_width);
    if (atlas_tiles_per_row == 0){
        std::cerr<<"Tiles per row error\n";
    }

    // Upload the complete palette lookup positions directly to VRAM
    for (size_t i = 0; i < data.palette_array.size(); ++i) {
        uint32_t index = data.palette_array[i].tileIndex;
        float pixelX = static_cast<float>(index % atlas_tiles_per_row) * data.texture_tile_width;
        float pixelY = static_cast<float>(index / atlas_tiles_per_row) * data.texture_tile_height;

        std::string uniformName = "u_Palette[" + std::to_string(i) + "]";
        shader.setUniform(uniformName.c_str(), Vec2f{pixelX, pixelY});
    }

    // Explicit float casts ensure your data maps perfectly onto the shader's vec2 registers
    shader.setUniform("u_TileSize", Vec2f{static_cast<float>(data.tile_width), static_cast<float>(data.tile_height)});
    shader.setUniform("u_TilesPerRow", static_cast<int>(data.columns));

    // Upload asset sizing parameters
    shader.setUniform("u_TextureSize", Vec2f{static_cast<float>(m_tileset_tex.getWidth()), static_cast<float>(m_tileset_tex.getHeight())});
    shader.setUniform("u_TextureTileSize", Vec2f{data.texture_tile_width, data.texture_tile_height});
    
    // Explicitly link the sampler identifier to texture slot index 0
    shader.setUniform("u_TilesetTexture", 0);
}

void TileMapRenderer::render(const TileMapData& data, Shader& shader, const WindowContext& context, bool isDirty) {
    if (data.map_grid.empty()) return;

    m_tileset_tex.use(0);
    shader.use();

    // ==========================================
    // OPTIMIZED DYNAMIC PATH GUARD
    // ==========================================
    if (isDirty) {
        glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
        glBufferData(GL_ARRAY_BUFFER, data.map_grid.size() * sizeof(uint32_t), data.map_grid.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Keep dimensions matched if structural data sizing changes at runtime
        shader.setUniform("u_TileSize", Vec2f{static_cast<float>(data.tile_width), static_cast<float>(data.tile_height)});
        shader.setUniform("u_TilesPerRow", static_cast<int>(data.columns));
    }

    // ==========================================
    // CAMERA AND VIEWPORT MATRIX RUNTIME SETUPS
    // ==========================================
    float left   = 0.0f;
    float right  = static_cast<float>(context.getWindow().getSize().width); 
    float bottom = static_cast<float>(context.getWindow().getSize().height); 
    float top    = 0.0f; // Forces 0,0 directly to the Top-Left corner of your screen
    float zNear  = -1.0f;
    float zFar   = 1.0f;

    glm::mat4 projection = glm::ortho(left, right, bottom, top, zNear, zFar); 
    glm::mat4 view       = glm::mat4(1.0f); // Identity camera framework representation

    // Upload calculations straight to your shader stage uniforms
    shader.setUniform("projection", projection);
    shader.setUniform("view", view);

    // ==========================================
    // EXECUTE DRAW
    // ==========================================
    glBindVertexArray(m_quad_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(data.map_grid.size()));
    glBindVertexArray(0);
}