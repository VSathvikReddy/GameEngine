#pragma once

#include <cstdint>

struct GLFWwindow;



class Window{
public:
    struct Size{
        uint32_t width = 0;
        uint32_t height = 0;
    };


    Window() = default;

    void setCallbacks(GLFWwindow* native);
    Size getFrameBufferSize() const;
    Size getWindowSize() const;
private:
    Size m_frame_buffer_size;
    Size m_window_size;
    void update_size(GLFWwindow* native);

    static void GLFW_framebuffer_size_callback(GLFWwindow* window, int width, int height); 
    static void GLFW_window_size_callback(GLFWwindow* window, int width, int height);
    static void GLFW_window_close_callback(GLFWwindow* window);
};