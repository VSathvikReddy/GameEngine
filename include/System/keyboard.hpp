#pragma once

#include <bitset>
#include <unordered_map>

struct GLFWwindow;
class KeyBind;

class Keyboard{
public:
    enum class Key : uint16_t;
    static const std::unordered_map<std::string_view, Key> StringToKeyMap;

    void EndFrame();

    //All the below rely on polling, callbacks are not implemented
    bool isKeyPressed(Key key) const;
    bool isKeyHeld(Key key) const;
    bool isKeyReleased(Key key) const;

    bool isKeyPressed(uint32_t id, KeyBind bind) const;
    bool isKeyHeld(uint32_t id, KeyBind bind)  const;
    bool isKeyReleased(uint32_t id, KeyBind bind)  const;

    void setCallbacks(GLFWwindow* window);
private:

    std::bitset<512> m_CurrentKeys;
    std::bitset<512> m_PreviousKeys;
    
    // The GLFW callback needs to be static to be passed as a C-pointer
    static void GLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};