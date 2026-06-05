#include "Shader/texture.hpp"
#include "Shader/texture_manager.hpp"


#include <GL/glew.h>

#include <cassert>

const Texture Texture::ERROR(0);

uint32_t Texture::getWidth() const noexcept {
    assert(TextureManager::master != nullptr && "TextureManager context was never registered!");
    return TextureManager::master->m_textures_properties[m_id].width;
}

uint32_t Texture::getHeight() const noexcept {
    assert(TextureManager::master != nullptr && "TextureManager context was never registered!");
    return TextureManager::master->m_textures_properties[m_id].height;
}

void Texture::use(unsigned int slot) const noexcept {
    assert(TextureManager::master != nullptr && "TextureManager context was never registered!");
    
    uint32_t glHandle = TextureManager::master->m_gpu_texture_ids[m_id];
    
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, glHandle);
}

Texture Texture::load(std::string_view path){
    return Texture(TextureManager::loadInternal(path));
}

