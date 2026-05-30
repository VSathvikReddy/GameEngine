#pragma once

#include "System/keyboard_keys.hpp"


#include <bitset>

struct GLFWwindow;
class KeyBind;

class Keyboard{
public:

    void EndFrame();

    //All the below rely on polling, callbacks are not implemented
    const bool isKeyPressed(Key key) const;
    const bool isKeyHeld(Key key) const;
    const bool isKeyReleased(Key key) const;

    const bool isKeyPressed(uint32_t id, KeyBind bind) const;
    const bool isKeyHeld(uint32_t id, KeyBind bind)  const;
    const bool isKeyReleased(uint32_t id, KeyBind bind)  const;

    void setCallbacks(GLFWwindow* window);
private:

    std::bitset<512> m_CurrentKeys;
    std::bitset<512> m_PreviousKeys;
    
    // The GLFW callback needs to be static to be passed as a C-pointer
    static void GLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};