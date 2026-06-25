#include "Render/Buffer/texture_array.hpp"
#include "stb_image.h" // Assumed image loading system library location
#include <iostream>
#include <cmath>

TextureArray::TextureArray(const std::vector<std::string>& file_paths){
    if (file_paths.empty()) {
        std::cerr << "Error: Cannot initialize an empty TextureArray.\n";
        return;
    }

    this->layer_count = static_cast<int>(file_paths.size());


    // 1. Probe the first image to establish standard array dimensions and channels
    stbi_set_flip_vertically_on_load(true);
    unsigned char* base_data = stbi_load(file_paths[0].c_str(), &width, &height, &nrChannels, 0);
    
    if (!base_data) {
        std::cerr << "Failed to load base asset: " << file_paths[0] << std::endl;
        return;
    }



    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);



    // Map format channels identically to your Texture class logic
    GLenum internal_format = getGLenumInternalFormat();
    GLenum data_format = getGLenumDataFormat();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 3. Immutable Allocation: Carve out the VRAM stack space block immediately
    int mip_levels = 1 + static_cast<int>(std::floor(std::log2(std::max(width, height))));
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mip_levels, internal_format, width, height, layer_count);

    // 4. Load the probed base slice into Layer Index 0
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, data_format, GL_UNSIGNED_BYTE, base_data);
    stbi_image_free(base_data);

    // 5. Sequentially stream subsequent assets into remaining layer slices
    for (int i = 1; i < layer_count; ++i) {
        int layer_w = 0, layer_h = 0, layer_ch = 0;
        unsigned char* layer_data = stbi_load(file_paths[i].c_str(), &layer_w, &layer_h, &layer_ch, 0);

        if (!layer_data) {
            std::cerr << "Failed to load subsequent texture index path: " << file_paths[i] << std::endl;
            continue;
        }
            // CRITICAL SANITY CHECK: Array dimensions must be strictly uniform
        if (layer_w != width || layer_h != height) {
            std::cerr << "CRITICAL WRONG FORMAT ERROR: File '" << file_paths[i] 
                        << "' size (" << layer_w << "x" << layer_h 
                        << ") fails to match array template requirements (" << width << "x" << height 
                        << "). Skipping layer!\n";
            stbi_image_free(layer_data);
            continue;
        }

        // Inject file pixel buffer data into targeted Layer index space 'i'
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, data_format, GL_UNSIGNED_BYTE, layer_data);
        stbi_image_free(layer_data);
    }

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    // 6. Apply configuration filters matching your engine style presets
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Useful only if clamp type is different
    // float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);   

    // For crisp retro tile pixels inside editors, NEAREST or LINEAR without mipmaps is ideal
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

TextureArray::~TextureArray() {
    if(ID!=0) glDeleteTextures(1, &ID); // Destroys the whole internal deck structure safely from VRAM
}

int TextureArray::getWidth() const{
    return width;
}
int TextureArray::getHeight() const{
    return height;
}
int TextureArray::getLayerCount() const {
    return layer_count;
}

void TextureArray::use(unsigned int slot) const {
    if(ID==0){ std::cerr<<"Broken texture being used\n"; return;}
    glActiveTexture(GL_TEXTURE0 + slot); // Keeps your array safe from random engine states!
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
}

GLenum TextureArray::getGLenumDataFormat(){
    switch(nrChannels){
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}

GLenum TextureArray::getGLenumInternalFormat(){
    switch(nrChannels){
        case 1: return GL_R8;
        case 3: return GL_RGB8;
        case 4: return GL_RGBA8;
        default: return GL_RGB8;
    }    
}



TextureArray::TextureArray(TextureArray&& other) noexcept: 
    ID(other.ID),width(other.width),height(other.height),nrChannels(other.nrChannels),layer_count(other.layer_count){
        other.ID = 0;
}
TextureArray& TextureArray::operator=(TextureArray&& other) noexcept{
    if (ID != 0) {
        glDeleteTextures(1, &ID);
    }
    ID = other.ID;
    width = other.width;
    height = other.height;
    nrChannels = other.nrChannels;
    layer_count = other.layer_count;

    other.ID = 0;

    return *this;
}