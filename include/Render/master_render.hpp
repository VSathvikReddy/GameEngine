#pragma once

#include "Render/Shader/shader_manager.hpp"
#include "Render/Texture/texture_manager.hpp"
#include <string_view>
#include <cstdint>

class MasterRenderer {
public:
    MasterRenderer();
    ~MasterRenderer() = default;

    // Rule of 5: Prevent accidental copies of the renderer system
    MasterRenderer(const MasterRenderer&) = delete;
    MasterRenderer& operator=(const MasterRenderer&) = delete;

    // --- Public Texture API Facade (Excludes 'use') ---
    void clearTextures() noexcept;
    
    [[nodiscard]] TextureID loadTexture(std::string_view path);
    [[nodiscard]] uint32_t getTextureWidth(TextureID id) const;
    [[nodiscard]] uint32_t getTextureHeight(TextureID id) const;

private:
    ShaderManager  m_shader_manager;
    TextureManager m_texture_manager;
};


inline MasterRenderer::MasterRenderer(): m_texture_manager(), m_shader_manager(){}

inline void MasterRenderer::clearTextures() noexcept {
    m_texture_manager.clear();
}

inline TextureID MasterRenderer::loadTexture(std::string_view path) {
    return m_texture_manager.loadTexture(path);
}

inline uint32_t MasterRenderer::getTextureWidth(TextureID id) const {
    return m_texture_manager.getWidth(id);
}

inline uint32_t MasterRenderer::getTextureHeight(TextureID id) const {
    return m_texture_manager.getHeight(id);
}