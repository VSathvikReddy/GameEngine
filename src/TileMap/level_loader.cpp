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



#include <iostream>
#include <iomanip> // For clean grid alignment formatting

void LevelLoader::printLevelData(const LevelData& data) {
    std::cout << "========================================\n";
    std::cout << "          LEVEL DATA VERIFICATION        \n";
    std::cout << "========================================\n";
    
    // 1. Print Core Configurations
    std::cout << "[CONFIG]\n";
    std::cout << "  Texture File:        " << data.texture_file << "\n";
    std::cout << "  Grid Dimensions:     " << data.columns << " columns x " << data.rows << " rows\n";
    std::cout << "  World Tile Size:     " << data.tile_width << "x" << data.tile_height << "\n";
    std::cout << "  Texture Tile Size:   " << data.texture_tile_width << "x" << data.texture_tile_height << "\n\n";

    // 2. Print Texture Palette Lookups
    std::cout << "[PALETTE ARRAY] (Total Entries: " << data.palette_array.size() << ")\n";
    for (size_t i = 0; i < data.palette_array.size(); ++i) {
        const auto& entry = data.palette_array[i];
        std::cout << "  [" << i << "] -> Atlas Tile Index: " << entry.tileIndex 
                  << " | Frames: " << entry.frameCount 
                  << " | Duration: " << entry.frameDuration << "s\n";
    }
    std::cout << "\n";

    // 3. Print the Map Grid as a structured 2D Matrix
    std::cout << "[MAP GRID VISUALIZATION]\n";
    if (data.map_grid.empty()) {
        std::cout << "  EMPTY GRID!\n";
    } else {
        for (uint32_t r = 0; r < data.rows; ++r) {
            std::cout << "  "; // Indent row
            for (uint32_t c = 0; c < data.columns; ++c) {
                size_t flatIndex = static_cast<size_t>(r) * data.columns + c;
                
                if (flatIndex < data.map_grid.size()) {
                    // std::setw(3) ensures columns align perfectly even with multi-digit IDs
                    std::cout << std::setw(3) << data.map_grid[flatIndex] << " ";
                }
            }
            std::cout << "\n";
        }
    }
    std::cout << "========================================\n";
}