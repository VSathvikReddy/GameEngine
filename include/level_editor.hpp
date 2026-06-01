#pragma once

#include "Shader/texture_array.hpp"

#include <cstdint>
#include <vector>
#include <memory>

class LevelEditor{
public:
    LevelEditor(const char* level);


    void setTile(uint32_t col, uint32_t row, uint32_t tile_id);
    uint32_t getTile(uint32_t col, uint32_t row) const;
private:
    using TileID = uint32_t;

    uint32_t columns,rows;
    float tile_width,tile_height;

    std::unique_ptr<TextureArray> tex;


    struct Palette{
        uint32_t layerID;
        float startX, startY;
        uint32_t width,height;
        uint32_t frameCount;
        float frameDuration;
    };
    
    std::vector<Palette> m_palette_array;
    std::vector<TileID> m_map;




    void loadLevelConfig(std::ifstream& file);
    void loadTextures(std::ifstream& file);
    void loadPaletts(std::ifstream& file);
    void loadMap(std::ifstream& file);
};