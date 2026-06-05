#pragma once

#include <string>
#include <unordered_map>
#include <vector>


#include "Shader/texture.hpp"

using SpriteSheetID = uint16_t;
using SpriteRegionID = uint32_t;
struct SpriteSheet{
    struct Region {
        float startX, startY;
        float width, height;
    };
    Texture texture;
    std::unordered_map<std::string, SpriteRegionID> stringToRegionIdMap;
    std::vector<Region> regions;
};

class SpriteSheetLoader {
public:
    static SpriteSheet loadFromFolder(const std::string& folderPath, const std::string& assetName);
};