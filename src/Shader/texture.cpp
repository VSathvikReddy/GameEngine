#include "Shader/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char* file_path){
    //stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file_path, &width, &height, &nrChannels, 0);

    if(!data){
        std::cerr << "Failed to load texture:"<<file_path<<std::endl;
        return;
    }




    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);


    GLenum internal_format = getGLenumInternalFormat(); // GPU storage (e.g., GL_RGBA8)
    GLenum data_format     = getGLenumDataFormat();     // CPU raw bytes  (e.g., GL_RGBA)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //tells OpenGL not to expect padding at the end of each image row,

    
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
    //set idx for uniform
    stbi_image_free(data);


    glGenerateMipmap(GL_TEXTURE_2D);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Useful only if clamp type is different
    //float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout<<file_path<<" loaded\n";
}

Texture::~Texture(){
    if (ID != 0) {
        glDeleteTextures(1, &ID);
    }
}

void Texture::use(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot); // Shifts dynamically (e.g., GL_TEXTURE0 + 2 = GL_TEXTURE2)
    glBindTexture(GL_TEXTURE_2D, ID);
}
int Texture::getWidth() const{
    return width;
}
int Texture::getHeight() const{
    return height;
}


GLenum Texture::getGLenumDataFormat(){
    switch(nrChannels){
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}

GLenum Texture::getGLenumInternalFormat(){
    switch(nrChannels){
        case 1: return GL_R8;
        case 3: return GL_RGB8;
        case 4: return GL_RGBA8;
        default: return GL_RGB8;
    }    
}
