#include "TileMap/level_loader.hpp"
#include "TileMap/level_data.hpp"

#include <iostream>
#include <fstream>

bool LevelLoader::load(const char* filepath, LevelData& outData){
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    outData.clear();



    std::string token;
    bool processing = false;

    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "LEVEL_CONFIG_START") { processing = true; continue; }
        if (token == "LEVEL_CONFIG_END")   { break; }

        if (processing) {
            if (token == "COLUMNS")                  file >> outData.columns;
            else if (token == "ROWS")                file >> outData.rows;
            else if (token == "TILE_WIDTH")          file >> outData.tile_width;
            else if (token == "TILE_HEIGHT")         file >> outData.tile_height;
            else if (token == "TEXTURE_TILE_WIDTH")  file >> outData.texture_tile_width;
            else if (token == "TEXTURE_TILE_HEIGHT") file >> outData.texture_tile_height;
            else if (token == "TEXTURE_FILE")        file >> outData.texture_file;
        }
    }



    

    processing = false;

    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "PALETTE_START") { processing = true; continue; }
        if (token == "PALETTE_END")   { break; }

        if (processing) {
            TilePalette entry;
            
            entry.tileIndex = std::stoul(token);
            file >> entry.frameCount >> entry.frameDuration;

            outData.palette_array.push_back(entry);

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









    while (file >> token) {
        if (token[0] == '#') { std::string discard; std::getline(file, discard); continue; }

        if (token == "GRID_START") { 
            outData.map_grid.resize(static_cast<size_t>(outData.columns) * outData.rows);
            for (size_t i = 0; i < outData.map_grid.size(); ++i) {
                file >> outData.map_grid[i];
            }
            continue; 
        }
        
        if (token == "GRID_END") { break; }
    }


    file.close();

    if (outData.texture_file.empty()) {
        return false;
    }

    return true;
}