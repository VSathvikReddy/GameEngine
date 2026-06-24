#include "System/keyboard.hpp"
#include "System/key_bindings.hpp"

#include "System/system.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>


void Keyboard::EndFrame(){
    m_PreviousKeys = m_CurrentKeys;
}






















void Keyboard::setCallbacks(GLFWwindow* window){
    glfwSetKeyCallback(window, Keyboard::GLFW_key_callback);
}

// The Static Bridge for polling
void Keyboard::GLFW_key_callback(GLFWwindow* window, int key,[[maybe_unused]] int scancode, int action,[[maybe_unused]] int mods) {

    Keyboard& instance = (static_cast<WindowContext*>(glfwGetWindowUserPointer(window))->m_keyboard);

    if (key < 0 || key >= 512) return;

    if (action == GLFW_PRESS){
        instance.m_CurrentKeys.set(key);
        
        // if (Keyboard->m_Callbacks.count(engineKey)) {
        //     for (auto& func : Keyboard->m_Callbacks[engineKey]) {
        //         func();
        //     }
        // }
    } 
    else if(action == GLFW_RELEASE){
        instance.m_CurrentKeys.reset(key);
    }
}






































// The compile-time FNV-1a Hash Algorithm
KeyBind::KeyBind(const char* file){
    std::fstream data(file);
    if(!data.is_open()){
        std::cerr<<"Unavle to open file for keybinds: "<<file<<'\n';
        exit(1);
    }
    std::string a,b,c;
    while((data>>a>>b>>c)){
        bindings[KeyBind::ID(a)] = Keyboard::StringToKeyMap.at(c);
    }
}

Keyboard::Key KeyBind::find(uint32_t id) const{
    auto ptr = this->bindings.find(id);
    if(ptr == bindings.end()){
        std::cerr<<"Unknow key Action\n";
        return Keyboard::Key::ERROR;
    }
    return ptr->second;
}
