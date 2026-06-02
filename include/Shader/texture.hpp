#pragma once

#include <GL/glew.h>

#include <iostream>
#include <cstdint>

class TextureArray;

class Texture{
public:
    Texture(const char* file_path);
    ~Texture();

    int getWidth() const;
    int getHeight() const;

    void use(unsigned int slot = 0) const;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) =delete;
    Texture& operator=(Texture&& other) = delete;

private:
    uint32_t ID =0;
    int width = 0, height=0, nrChannels=0;

    GLenum getGLenumDataFormat();
    GLenum getGLenumInternalFormat();
};