#pragma once

#include <cstdint>

struct GLFWwindow;

struct WindowProperties{
    uint32_t width = 0;
    uint32_t height = 0;
    const char* title = "Default Name";
};

class Window{
public:
    Window(const WindowProperties& properties);

    void setCallbacks(GLFWwindow* native);
    const WindowProperties& getProperties() const;
private:
    WindowProperties m_properties;


    static void GLFW_framebuffer_size_callback(GLFWwindow* window, int width, int height); 
};