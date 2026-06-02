#pragma once

struct TileMapData;

class TileMapLoader {
public:
    static bool load(const char* filepath, TileMapData& outData);
    static bool save(const char* filepath, const TileMapData& data);


    static void printTileMapData(const TileMapData& data);
};