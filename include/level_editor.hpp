#pragma once

#include "Shader/texture.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

class LevelEditor {
public:
    LevelEditor(const char* level);
    ~LevelEditor() = default;

    void setTile(uint32_t col, uint32_t row, uint32_t tile_id);
    uint32_t getTile(uint32_t col, uint32_t row) const;

    std::pair<float,float> calculateTileCoordinates(uint32_t tile_id, uint32_t active_frame) const;

private:
    using TileID = uint32_t;

    void loadLevelConfig(std::ifstream& file);
    void loadPaletts(std::ifstream& file); // Updated to push entries sequentially
    void loadMap(std::ifstream& file);

    uint32_t columns = 0, rows = 0;
    float tile_width = 32.0f, tile_height = 32.0f;
    float texture_tile_width = 32.0f, texture_tile_height = 32.0f;

    std::string texture_file;
    uint32_t tiles_per_row;

    std::unique_ptr<Texture> tex;

    struct Palette {
        uint32_t tileIndex;
        uint32_t frameCount;
        float frameDuration;
    };
    
    std::vector<Palette> m_palette_array;
    std::vector<TileID> m_map;
};