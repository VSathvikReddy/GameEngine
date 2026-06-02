#include "System/system.hpp"

#include "TileMap/level_loader.hpp"
#include "TileMap/level_data.hpp"

#include "Shader/shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>


// ${workspaceFolder}/build/compile_commands.json Complier commands
// Cmake again
int main(){
    //Increase Size not working
    WindowContext context({1000,1000,"Hello WOrld"});

    Shader shd("Shader/tilemap.vert", "Shader/tilemap.frag");
    Texture tex("assets/test.png");

    while(context.isOpen()){
        context.startFrame();

        // ren.render(lvl,true,shd,context);
        


        context.endFrame();
    }

    return 0;
}