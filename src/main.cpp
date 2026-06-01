#include "System/system.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>


// ${workspaceFolder}/build/compile_commands.json Complier commands
// Cmake again
int main(){
    //Increase Size not working
    WindowContext context({1000,1000,"Hello WOrld"});

    while(context.isOpen()){
        context.startFrame();

        if(context.getKeyboard().isKeyPressed(Key::W)){
            std::cout<<"W\n";
        }


        context.endFrame();
    }

    return 0;
}