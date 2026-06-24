#include "System/system.hpp"

#include <iostream>
#include <thread> // Required for std::this_thread::sleep_for
#include <chrono> // Required for std::chrono durations

int main(){
    WindowContext window(500,500,"Name");
    
     std::cout<<window.getWindow().getFrameBufferSize().width<<' '<<window.getWindow().getFrameBufferSize().width<<'\n';
     std::cout<<window.getWindow().getFrameBufferSize().width<<' '<<window.getWindow().getFrameBufferSize().width<<'\n';

    bool first_frame = true;
    while(window.isOpen()){
        window.startFrame();
        
        std::cout<<window.getMouse().getPosition().x<<' '<<window.getMouse().getPosition().y<<'\n';
        std::cout<<window.getMouse().getDelta().x<<' '<<window.getMouse().getDelta().y<<'\n';

        if(first_frame){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            first_frame = false;
        }
        

        window.endFrame();
    }

    return 0;
}