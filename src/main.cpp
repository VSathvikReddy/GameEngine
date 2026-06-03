#include "System/system.hpp"
#include "Shader/shader.hpp"
#include "Shader/texture.hpp"
#include "Shader/texture_manager.hpp"

#include "TileMap/tilemap_data.hpp"
#include "TileMap/tilemap_loader.hpp"
#include "TileMap/tilemap_renderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {
    TextureManager manager;
    TextureManager::RegisterManager(&manager);
    WindowContext context({1000, 1000, "Hello Raw NDC"});

    Shader shd("Shader/tilemap.vert", "Shader/tilemap.frag");
    Texture tex = Texture::load("assets/test.png");

    TileMapData map("assets/map.txt");
    TileMapRenderer rdr(map,shd,context);

    while (context.isOpen()){
        context.startFrame();

        rdr.render(map,shd,context,false);

        context.endFrame();
    }



    return 0;
}