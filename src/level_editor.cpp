#include "level_editor.hpp"
#include <iostream>

LevelEditor::LevelEditor(const char* level) {
    std::ifstream file(level);
    if (!file.is_open()) {
        std::cerr << "LevelEditor Error: Failed to open file: " << level << "\n";
        return;
    }

    loadLevelConfig(file);
    loadPaletts(file);
    loadMap(file);
    file.close();

    if (!texture_file.empty()) {
        this->tex = std::make_unique<Texture>(texture_file.c_str());
        this->tiles_per_row =  tex->getWidth() / static_cast<int>(texture_tile_width);
    }
}

void LevelEditor::loadLevelConfig(std::ifstream& file) {
    std::string token;
    bool processing = false;

    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "LEVEL_CONFIG_START") { processing = true; continue; }
        if (token == "LEVEL_CONFIG_END")   { break; }

        if (processing) {
            if (token == "COLUMNS")                  file >> columns;
            else if (token == "ROWS")                file >> rows;
            else if (token == "TILE_WIDTH")          file >> tile_width;
            else if (token == "TILE_HEIGHT")         file >> tile_height;
            else if (token == "TEXTURE_TILE_WIDTH")  file >> texture_tile_width;
            else if (token == "TEXTURE_TILE_HEIGHT") file >> texture_tile_height;
            else if (token == "TEXTURE_FILE")        file >> texture_file;
        }
    }
}

void LevelEditor::loadPaletts(std::ifstream& file) {
    std::string token;
    bool processing = false;

    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "PALETTE_START") { processing = true; continue; }
        if (token == "PALETTE_END")   { break; }

        if (processing) {
            Palette entry;
            
            entry.tileIndex = std::stoul(token);
            file >> entry.frameCount >> entry.frameDuration;

            m_palette_array.push_back(entry);

            // Safely swallow trailing descriptive code comments
            std::string comment_check;
            auto current_pos = file.tellg();
            if (file >> comment_check && comment_check[0] == '#') {
                std::getline(file, comment_check);
            } else {
                file.seekg(current_pos);
            }
        }
    }
}

void LevelEditor::loadMap(std::ifstream& file) {
    std::string token;

    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "GRID_START") { 
            m_map.resize(static_cast<size_t>(columns) * rows);
            for (size_t i = 0; i < m_map.size(); ++i) {
                file >> m_map[i];
            }
            continue; 
        }
        
        if (token == "GRID_END") { break; }
    }
}

std::pair<float,float> LevelEditor::calculateTileCoordinates(uint32_t tile_id, uint32_t active_frame) const {
    // The tile_id from the map grid maps directly to the array vector index slot!
    if (tile_id >= m_palette_array.size() || !tex) {
        std::cerr<<tile_id<<" : out of range\n";
        return;
    }
    
    // Horizontal tracking calculation to drive real-time animation frame parsing
    uint32_t target_index = m_palette_array[tile_id].tileIndex + active_frame;

    return{
        static_cast<float>(target_index % tiles_per_row) * texture_tile_width,
        static_cast<float>(target_index / tiles_per_row) * texture_tile_height
    };
}

void LevelEditor::setTile(uint32_t col, uint32_t row, uint32_t tile_id) {
    if (col >= columns || row >= rows) return;
    m_map[(static_cast<size_t>(row) * columns) + col] = tile_id;
}

uint32_t LevelEditor::getTile(uint32_t col, uint32_t row) const {
    if (col >= columns || row >= rows) return 0;
    return m_map[(static_cast<size_t>(row) * columns) + col];
}