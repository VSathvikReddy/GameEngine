#include "System/system.hpp"
#include "Shader/shader.hpp"
#include "Shader/texture.hpp"

#include "TileMap/tilemap_data.hpp"
#include "TileMap/tilemap_loader.hpp"
#include "TileMap/tilemap_renderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {
    WindowContext context({1000, 1000, "Hello Raw NDC"});

    // Load our basic standalone pass-through shaders
    Shader shd("Shader/tilemap.vert", "Shader/tilemap.frag");
    Texture tex("assets/test.png");

    TileMapData map("assets/map.txt");
    TileMapRenderer rdr(map,shd,context);

    while (context.isOpen()) {
        context.startFrame();

        rdr.render(map,shd,context,false);

        context.endFrame();
    }



    return 0;
}