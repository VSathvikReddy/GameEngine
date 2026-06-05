#include "Render/Sprite/sprite_sheet.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem> // C++17 modern path handling
#include <glm/glm.hpp>

namespace fs = std::filesystem;

SpriteSheet SpriteSheetLoader::loadFromFolder(const std::string& folderPath, const std::string& assetName) {

    // 1. Build strict cross-platform filesystem paths
    fs::path baseDir(folderPath);
    fs::path texturePath  = baseDir / (assetName + ".png");
    fs::path metadataPath = baseDir / (assetName + ".meta");

    if (!fs::exists(texturePath)){
        std::cerr << "Error: Texture image file not found at " << texturePath << "\n";
        return {Texture::ERROR};
    }
    Texture texture = Texture::load(texturePath.string());
    SpriteSheet sheet{texture};

    std::ifstream file(metadataPath);
    if (!file.is_open()) {
        std::cerr << "Error: Metadata manifest file not found at " << metadataPath << "\n";
        return sheet;
    }

    sheet.regions.clear();
    sheet.stringToRegionIdMap.clear();
    
    uint32_t uniqueIdCounter = 0;
    bool inPaletteZone = false;
    std::string line;

    while (std::getline(file, line)){
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        if (line.find("PALETTE_START") != std::string::npos) {
            inPaletteZone = true;
            continue;
        }
        if (line.find("PALETTE_END") != std::string::npos) {
            inPaletteZone = false;
            break; // Safely stop parsing early
        }

        if (inPaletteZone) {
            std::stringstream ss(line);
            std::string key;
            char equalsSign;
            float x, y, w, h;

            if (ss >> key >> equalsSign >> x >> y >> w >> h) {
                if (uniqueIdCounter >= 256) {
                    std::cerr << "Warning: Exceeded maximum shader registry capacity (256 items).\n";
                    break;
                }

                // Create the region boundaries
                SpriteSheet::Region region = { x, y, w, h };
                sheet.regions.push_back(region);

                // Document the string lookup configuration assignment
                sheet.stringToRegionIdMap[key] = uniqueIdCounter;
                
                uniqueIdCounter++;
            } else {
                std::cerr << "Warning: Failed to parse metadata entry line: " << line << "\n";
            }
        }
    }

    file.close();
    return sheet;
}