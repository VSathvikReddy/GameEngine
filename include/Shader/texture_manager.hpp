#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cstdint>

class Texture;

class TextureManager {
public:
    TextureManager();    
    ~TextureManager();

    static void RegisterManager(TextureManager* instance) noexcept { master = instance; }
    static void clear() noexcept;

    friend class Texture;
private:
    struct TextureProperties {
        uint32_t width  = 0; 
        uint32_t height = 0;
        constexpr TextureProperties(uint32_t w, uint32_t h) noexcept : width(w), height(h) {}
        constexpr TextureProperties() noexcept = default;
    };

    std::vector<TextureProperties> m_textures_properties;
    std::vector<uint32_t>          m_gpu_texture_ids;
    std::unordered_map<std::string, uint32_t> m_name_to_ID;

    static uint32_t loadInternal(std::string_view path);

    void initializeDefaultTexture();

    inline static TextureManager* master = nullptr;
};