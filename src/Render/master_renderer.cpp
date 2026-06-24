#include "Render/master_render.hpp"

inline MasterRenderer::MasterRenderer(): m_texture_manager(), m_shader_manager(){}

inline void MasterRenderer::clearTextures() noexcept {
    m_texture_manager.clear();
}

inline TextureID MasterRenderer::loadTexture(const std::string& path) {
    return m_texture_manager.loadTexture(path);
}

inline uint32_t MasterRenderer::getTextureWidth(TextureID id) const {
    return m_texture_manager.getWidth(id);
}

inline uint32_t MasterRenderer::getTextureHeight(TextureID id) const {
    return m_texture_manager.getHeight(id);
}