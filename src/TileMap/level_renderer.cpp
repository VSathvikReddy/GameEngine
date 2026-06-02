#include "TileMap/level_renderer.hpp"
#include "System/system.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <iostream>
#include <string>


LevelRenderer::LevelRenderer(const LevelData& data, Shader& shader) 
    : m_tileset_tex(data.texture_file.c_str()) {
    initRenderData(data, shader);
}

LevelRenderer::~LevelRenderer() {
    if (m_quad_vao != 0) glDeleteVertexArrays(1, &m_quad_vao);
    if (m_quad_vbo != 0) glDeleteBuffers(1, &m_quad_vbo);
    if (m_id_vbo != 0) glDeleteBuffers(1, &m_id_vbo);
}

void LevelRenderer::initRenderData(const LevelData& data, Shader& shader) {
    float vertices[] = { 
        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 0.0f,     0.0f, 0.0f, 

        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 1.0f,     1.0f, 1.0f,
        1.0f, 0.0f,     1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);
    glGenBuffers(1, &m_id_vbo);

    glBindVertexArray(m_quad_vao);

    // STATIC QUAD GEOMETRY PIPELINE
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // DYNAMIC TILE INSTANCE IDs PIPELINE
    glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
    glBufferData(GL_ARRAY_BUFFER, data.map_grid.size() * sizeof(uint32_t), data.map_grid.data(), GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(uint32_t), (void*)0); //I for int
    glVertexAttribDivisor(2, 1); 


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ==========================================
    // INITIALIZATION UNIFORM UPLOAD (RUNS ONCE)
    // ==========================================
    shader.use();

    int atlas_tiles_per_row = m_tileset_tex.getWidth() / static_cast<int>(data.texture_tile_width);
    if (atlas_tiles_per_row == 0) atlas_tiles_per_row = 1;

    // Upload the entire palette lookup table to VRAM here
    for (size_t i = 0; i < data.palette_array.size(); ++i) {
        uint32_t index = data.palette_array[i].tileIndex;
        float pixelX = static_cast<float>(index % atlas_tiles_per_row) * data.texture_tile_width;
        float pixelY = static_cast<float>(index / atlas_tiles_per_row) * data.texture_tile_height;

        std::string uniformName = "u_Palette[" + std::to_string(i) + "]";
        shader.setUniform(uniformName.c_str(),pixelX,pixelY);
    }

    // Upload invariant texture dimensional metrics
    shader.setUniform("u_TextureSize",static_cast<float>(m_tileset_tex.getWidth()), static_cast<float>(m_tileset_tex.getHeight()));
    shader.setUniform("u_TextureTileSize", data.texture_tile_width, data.texture_tile_height);
}

void LevelRenderer::render(LevelData& data, bool isDirty, Shader& shader, const WindowContext& context) {
    if (data.map_grid.empty()) return;

    m_tileset_tex.use(0);
    shader.use();

    // ==========================================
    // OPTIMIZED DYNAMIC PATH GUARD
    // ==========================================
    // Palette assignments are completely gone from here!
    if (isDirty) {
        glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
        glBufferData(GL_ARRAY_BUFFER, data.map_grid.size() * sizeof(uint32_t), data.map_grid.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Update map dimension changes (retains functionality if the editor changes grid size)
        shader.setUniform("u_TileSize",data.tile_width, data.tile_height);
        shader.setUniform("u_TilesPerRow",data.columns);
    }
// Generate an orthographic 2D map projection matrix (Y-Down, top-left 0,0 alignment matching screen layout)
// 1. Extract and explicitly convert window sizes to floats
    float left   = 0.0f;
    float right  = static_cast<float>(context.getWindow().getSize().width); // or .width depending on your class
    float bottom = static_cast<float>(context.getWindow().getSize().height); // or .height depending on your class
    float top    = 0.0f;
    float zNear  = -1.0f;
    float zFar   = 1.0f;

// 2. Pass the purely uniform float variables into the matrix generator
glm::mat4 projection = glm::ortho(left, right, bottom, top, zNear, zFar); 

    // Create base identity view tracking position frame
    glm::mat4 view = glm::mat4(1.0f);

    // Safely send calculated systems into registration slots using your template specialization engines
    // shader.setUniform("projection", &projection[0][0]);
    // shader.setUniform("view", &view[0][0]);
    // ==========================================
    // EXECUTE DRAW
    // ==========================================
    glBindVertexArray(m_quad_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(data.map_grid.size()));
    glBindVertexArray(0);
}