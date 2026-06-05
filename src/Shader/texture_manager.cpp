#include "Render/Texture/texture_manager.hpp"
#include "Render/Texture/texture.hpp"

#include <iostream>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>


TextureManager::TextureManager() {
    initializeDefaultTexture();
}

TextureManager::~TextureManager() {
    // Delete ALL textures on the GPU including the default texture at index 0
    if (!m_gpu_texture_ids.empty()) {
        glDeleteTextures(static_cast<GLsizei>(m_gpu_texture_ids.size()), m_gpu_texture_ids.data());
    }
    
    m_textures_properties.clear();
    m_gpu_texture_ids.clear();
    m_name_to_ID.clear();
}

void TextureManager::initializeDefaultTexture() {
    if (!m_gpu_texture_ids.empty()) return;

    const uint8_t magentaBlackPixels[] = {
        255,   0, 255, 255,     0,   0,   0, 255, // Row 1: Magenta, Black
          0,   0,   0, 255,   255,   0, 255, 255  // Row 2: Black, Magenta
    };

    uint32_t fallback_gl_id = 0;
    glGenTextures(1, &fallback_gl_id);
    glBindTexture(GL_TEXTURE_2D, fallback_gl_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, magentaBlackPixels);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Register it explicitly into index 0
    m_textures_properties.emplace_back(2, 2);
    m_gpu_texture_ids.push_back(fallback_gl_id);
    m_name_to_ID["__fallback_error__"] = 0;
}

// =====================================================================
// SMART CLEAR (Clears all loaded assets EXCEPT the default texture)
// =====================================================================
void TextureManager::clear() noexcept {
    if (!master || master->m_gpu_texture_ids.size() <= 1) return;

    // Delete all game textures from the GPU (everything EXCEPT index 0)
    GLsizei textureCountToDelete = static_cast<GLsizei>(master->m_gpu_texture_ids.size() - 1);
    glDeleteTextures(textureCountToDelete, &master->m_gpu_texture_ids[1]);

    master->m_textures_properties.resize(1);
    master->m_gpu_texture_ids.resize(1);

    master->m_name_to_ID.clear();
    master->m_name_to_ID["__fallback_error__"] = 0;
}
uint32_t TextureManager::loadInternal(std::string_view path) {
    assert(master != nullptr && "TextureManager instance was never registered!");

    std::string key(path);
    auto it = master->m_name_to_ID.find(key);
    if (it != master->m_name_to_ID.end()) {
        return it->second;
    }

    int width = 0;
    int height = 0;
    int nrChannels = 0;

    // stbi_set_flip_vertically_on_load(true); // Uncomment if your engine requires UV flips
    unsigned char* data = stbi_load(key.c_str(), &width, &height, &nrChannels, 0);

    if (!data){
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    uint32_t gl_texture_id = 0;
    glGenTextures(1, &gl_texture_id);
    glBindTexture(GL_TEXTURE_2D, gl_texture_id);

    // Default 3 this
    GLenum internal_format = GL_RGB8;
    GLenum data_format = GL_RGB;

    if (nrChannels == 4) {
        internal_format = GL_RGBA8;
        data_format = GL_RGBA;
    } else if (nrChannels == 1) {
        internal_format = GL_R8;
        data_format = GL_RED;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //tells OpenGL not to expect padding at the end of each image row,
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);

    glGenerateMipmap(GL_TEXTURE_2D);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Useful only if clamp type is different
    //float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 4. Save data directly into our parallel columns
    uint32_t assigned_id = static_cast<uint32_t>(master->m_gpu_texture_ids.size());


    master->m_textures_properties.emplace_back(static_cast<uint32_t>(width),static_cast<uint32_t>(height));
    master->m_gpu_texture_ids.push_back(gl_texture_id);
    
    master->m_name_to_ID[key] = assigned_id;

    return assigned_id;
}