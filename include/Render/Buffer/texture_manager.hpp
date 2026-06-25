#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cstdint>
#include <iostream>

#include "Render/Buffer/gl_alias.hpp"

class TextureManager {
public:
    TextureManager();    
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    void clear() noexcept;
    TextureID loadTexture(const std::string& path);

    uint32_t getWidth(TextureID id) const;
    uint32_t getHeight(TextureID id) const;

    void use(TextureID id, int slot = 0) const;

private:
    struct TextureProperties {
        uint32_t width  = 0; 
        uint32_t height = 0;
    };

    std::vector<TextureProperties> m_textures_properties;
    std::vector<uint32_t>          m_gpu_texture_ids;
    std::unordered_map<std::string, uint32_t> m_name_to_ID;

    void initializeDefaultTexture();
};


inline uint32_t TextureManager::getWidth(TextureID id) const {
#ifndef NDEBUG
    if (id >= m_gpu_texture_ids.size()) {
        std::cerr << "[TextureManager] Warning: Invalid ID " << id << " requested. Using fallback.\n";
        id = ERROR_TEXTURE; 
    }
#endif
    return m_textures_properties[id].width;
}

inline uint32_t TextureManager::getHeight(TextureID id) const {
#ifndef NDEBUG
    if (id >= m_gpu_texture_ids.size()) {
        std::cerr << "[TextureManager] Warning: Invalid ID " << id << " requested. Using fallback.\n";
        id = ERROR_TEXTURE; 
    }
#endif
    return m_textures_properties[id].height;
}