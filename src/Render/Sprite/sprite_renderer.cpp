#include "Render/Sprite/sprite_renderer.hpp"
#include "Render/Sprite/sprite.hpp"

#include "Shader/shader.hpp"

#include <GL/glew.h>

#include <cmath>
#include <algorithm>

extern Renderer<Sprite>* g_ActiveRenderer;

Renderer<Sprite>::Renderer(const Shader& compatibleShader) {
    float quadVertices[] = {
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 0.0f,  0.0f, 0.0f,

        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);
    glGenBuffers(1, &m_id_vbo);

    glBindVertexArray(m_quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));





    glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_CAPACITY * sizeof(SpriteGPUData), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteGPUData), (void*)offsetof(SpriteGPUData, position));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteGPUData), (void*)offsetof(SpriteGPUData, right));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteGPUData), (void*)offsetof(SpriteGPUData, up));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(SpriteGPUData), (void*)offsetof(SpriteGPUData, regionID));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SpriteGPUData), (void*)offsetof(SpriteGPUData, color));
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);

    glGenBuffers(1, &m_uboPaletteBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboPaletteBlock);
    glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Renderer<Sprite>::~Renderer() {
    glDeleteVertexArrays(1, &m_quad_vao);
    glDeleteBuffers(1, &m_quad_vbo);
    glDeleteBuffers(1, &m_id_vbo);
    glDeleteBuffers(1, &m_uboPaletteBlock);
}

void Renderer<Sprite>::loadSpriteSheet(const std::string& folderPath, const std::string& assetName) {
    if (m_sheetPathRegistry.find(assetName) != m_sheetPathRegistry.end()) {
        return;
    }

    m_sheets.push_back(SpriteSheetLoader::loadFromFolder(folderPath, assetName));
    
    uint16_t assignedID = static_cast<uint16_t>(m_sheets.size() - 1);
    m_sheetPathRegistry[assetName] = assignedID;
}

void Renderer<Sprite>::acceptRenderOrder(const Sprite& object) {
    if (!object.isValid()) return;

    const SpriteData& data = actual_data.at(object.id);

    float rad = data.rotation.asRadians();
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    SpriteGPUData gpuData;
    gpuData.position = data.position;
    gpuData.right    = Vec2f{ cosA * data.dimensions.x, sinA * data.dimensions.x };
    gpuData.up       = Vec2f{ -sinA * data.dimensions.y, cosA * data.dimensions.y };
    gpuData.regionID = static_cast<SpriteRegionID>(data.regionID);
    gpuData.color    = data.color;

    m_buckets[data.sheetID].push_back(gpuData);
}

void Renderer<Sprite>::render(Shader& shader) {
    shader.use();

    glBindVertexArray(m_quad_vao);

    for (auto& [sheetID, instanceList] : m_buckets) {
        if (instanceList.empty()) continue;

        const SpriteSheet& sheet = m_sheets[sheetID];
        
        float paletteData[256 * 4] = {0.0f};
        size_t regionsToUpload = std::min(sheet.regions.size(), size_t(256));
        for (size_t i = 0; i < regionsToUpload; ++i) {
            paletteData[i * 4 + 0] = sheet.regions[i].startX;
            paletteData[i * 4 + 1] = sheet.regions[i].startY;
            paletteData[i * 4 + 2] = sheet.regions[i].width;
            paletteData[i * 4 + 3] = sheet.regions[i].height;
        }
        glBindBuffer(GL_UNIFORM_BUFFER, m_uboPaletteBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(paletteData), paletteData);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboPaletteBlock);

        size_t instancesLeft = instanceList.size();
        size_t offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
        while (instancesLeft > 0) {
            size_t batchSize = std::min(instancesLeft, MAX_BATCH_CAPACITY);

            glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_CAPACITY * sizeof(SpriteGPUData), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, batchSize * sizeof(SpriteGPUData), &instanceList[offset]);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(batchSize));

            offset += batchSize;
            instancesLeft -= batchSize;
        }

        instanceList.clear();
    }
}