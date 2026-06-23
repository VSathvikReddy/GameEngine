#pragma once

// Work in Progress, restructure
// #include "Render/Texture/texture.hpp"

#include <GL/glew.h>

#include <vector>
#include <cstdint>

class TextureArray{
public:
    // Accepts a vector of file paths to build the stacked deck of textures
    TextureArray(const std::vector<std::string>& file_paths);
    ~TextureArray();

    int getWidth() const;
    int getHeight() const;
    int getLayerCount() const;

    // Binds this texture array configuration block to the active state
    void use(unsigned int slot = 0) const;

    TextureArray(const TextureArray&) = delete;
    TextureArray& operator=(const TextureArray&) = delete;

    TextureArray(TextureArray&& other) noexcept;
    TextureArray& operator=(TextureArray&& other) noexcept;
private:
    uint32_t ID = 0;
    int width, height, nrChannels;
    int layer_count;

    GLenum getGLenumDataFormat();
    GLenum getGLenumInternalFormat();
};