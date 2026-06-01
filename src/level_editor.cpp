#include "level_editor.hpp"

#include <fstream>
#include <iostream>

LevelEditor::LevelEditor(const char* level) {
    std::ifstream file(level);
    if (!file.is_open()) {
        std::cerr << "LevelEditor Error: Failed to open file: " << level << "\n";
        return;
    }

    loadLevelConfig(file);
    loadTextures(file);
    loadPaletts(file);
    loadMap(file);

    file.close();

}

void LevelEditor::loadLevelConfig(std::ifstream& file) {
    std::string token;
    bool processing = false;

    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "LEVEL_CONFIG_START") { processing = true; continue; }
        if (token == "LEVEL_CONFIG_END")   { break; }

        if (processing) {
            if (token == "COLUMNS")          file >> columns;
            else if (token == "ROWS")        file >> rows;
            else if (token == "TILE_WIDTH")  file >> tile_width;
            else if (token == "TILE_HEIGHT") file >> tile_height;
        }
    }
}

void LevelEditor::loadTextures(std::ifstream& file) {
    std::vector<std::string> texture_paths;
    std::string token;
    bool processing = false;

    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "TEXTURES_START") { processing = true; continue; }
        if (token == "TEXTURES_END")   { break; }

        if (processing) {
            size_t layer_idx = std::stoull(token);
            std::string path;
            file >> path;

            if (layer_idx >= texture_paths.size()) {
                texture_paths.resize(layer_idx + 1);
            }
            texture_paths[layer_idx] = path;

            std::string comment_check;
            auto current_pos = file.tellg();
            if (file >> comment_check && comment_check[0] == '#') {
                std::getline(file, comment_check);
            } else {
                file.seekg(current_pos);
            }
        }
    }

    if (!texture_paths.empty()) {
        tex = std::make_unique<TextureArray>(texture_paths);
        std::cout << "LevelEditor: Loaded " << texture_paths.size() << " layers into TextureArray.\n";
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
            uint32_t id = std::stoul(token);
            Palette entry;
            
            file >> entry.layerID >> entry.startX >> entry.startY 
                 >> entry.width >> entry.height >> entry.frameCount >> entry.frameDuration;

            if (id >= m_palette_array.size()) {
                m_palette_array.resize(id + 1);
            }
            m_palette_array[id] = entry;

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

void LevelEditor::setTile(uint32_t col, uint32_t row, uint32_t tile_id) {
    if (col >= columns || row >= rows) return;
    m_map[(static_cast<size_t>(row) * columns) + col] = tile_id;
}

uint32_t LevelEditor::getTile(uint32_t col, uint32_t row) const {
    if (col >= columns || row >= rows) return 0;
    return m_map[(static_cast<size_t>(row) * columns) + col];
}