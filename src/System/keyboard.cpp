#include "System/keyboard.hpp"
#include "System/key_bindings.hpp"

#include "System/system.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>


void Keyboard::EndFrame(){
    m_PreviousKeys = m_CurrentKeys;
}

bool Keyboard::isKeyHeld(Key key) const {
    return m_CurrentKeys.test(static_cast<size_t>(key));
}bool Keyboard::isKeyPressed(Key key) const {
    int k = static_cast<int>(key);
    return m_CurrentKeys.test(k) && !m_PreviousKeys.test(k);
}bool Keyboard::isKeyReleased(Key key) const {
    int k = static_cast<int>(key);
    return !m_CurrentKeys.test(k) && m_PreviousKeys.test(k);
}
// void Keyboard::Subscribe(Key key, EventCallback callback) {
//     m_Callbacks[key].push_back(callback);
// }

bool Keyboard::isKeyHeld(uint32_t id, KeyBind bind)  const {
    return m_CurrentKeys.test(static_cast<size_t>(bind.find(id)));
}bool Keyboard::isKeyPressed(uint32_t id, KeyBind bind)  const {
    int k = static_cast<int>(bind.find(id));
    return m_CurrentKeys.test(k) && !m_PreviousKeys.test(k);
}bool Keyboard::isKeyReleased(uint32_t id, KeyBind bind)  const {
    int k = static_cast<int>(bind.find(id));
    return !m_CurrentKeys.test(k) && m_PreviousKeys.test(k);
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







































KeyBind::KeyBind(const char* file){
    std::fstream data(file);
    if(!data.is_open()){
        std::cerr<<"Unavle to open file for keybinds: "<<file<<'\n';
        exit(1);
    }
    std::string a,b,c;
    while((data>>a>>b>>c)){
        bindings[KeyBind::ID(a)] = StringToKeyMap.at(c);
    }
}

Key KeyBind::find(uint32_t id){
    auto ptr = this->bindings.find(id);
    if(ptr == bindings.end()){
        std::cerr<<"Unknow key Action\n";
        return Key::ERROR;
    }
    return ptr->second;
}
