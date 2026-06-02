#pragma once

#include "level_data.hpp"

class LevelLoader {
public:
    static bool load(const char* filepath, LevelData& outData);
    static bool save(const char* filepath, const LevelData& data);


    static void printLevelData(const LevelData& data);
};