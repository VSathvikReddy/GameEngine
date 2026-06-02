#include "TileMap/tilemap_data.hpp"
#include "TileMap/tilemap_loader.hpp"

#include <iostream>

TileMapData::TileMapData(const char* file){
    if(!TileMapLoader::load(file,*this)){
        std::cerr<<"Failed to load tilemap: "<<file<<'\n';
    }
}


void TileMapData::clear(){
    texture_file.clear();
    palette_array.clear();
    map_grid.clear();
    columns = rows = 0;
}