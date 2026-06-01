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

private:
    uint32_t ID;
    int width, height, nrChannels;

    GLenum getGLenumDataFormat();
    GLenum getGLenumInternalFormat();
};