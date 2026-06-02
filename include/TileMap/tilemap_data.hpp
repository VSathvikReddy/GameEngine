#pragma once


#include <vector>
#include <string>
#include <cstdint>

struct TilePalette {
    uint32_t tileIndex;
    uint32_t frameCount;
    float frameDuration;
};

struct TileMapData {
    TileMapData(const char* file);
    uint32_t columns = 0, rows = 0;

    float tile_width = 32.0f, tile_height = 32.0f;
    float texture_tile_width = 32.0f, texture_tile_height = 32.0f;

    std::string texture_file;

    std::vector<TilePalette> palette_array;
    std::vector<uint32_t> map_grid;

    
    void clear();
};
