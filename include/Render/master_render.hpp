#pragma once

#include "Render/Shader/shader_manager.hpp"
#include "Render/Texture/texture_manager.hpp"
#include <string_view>
#include <cstdint>

class MasterRenderer {
public:
    MasterRenderer();
    ~MasterRenderer() = default;

    MasterRenderer(const MasterRenderer&) = delete;
    MasterRenderer& operator=(const MasterRenderer&) = delete;

    void clearTextures() noexcept;
    
    [[nodiscard]] TextureID loadTexture(const std::string& path);
    [[nodiscard]] uint32_t getTextureWidth(TextureID id) const;
    [[nodiscard]] uint32_t getTextureHeight(TextureID id) const;

    template<typename T> IRenderSystem* createRenderSystem();

private:
    ShaderManager  m_shader_manager;
    TextureManager m_texture_manager;
};


class IRenderSystem{
public:
    virtual void draw();
    void setup();
private:
    ShaderID m_required_shader;
};