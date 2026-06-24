#include "System/window.hpp"
#include "System/system.hpp"
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>

void Window::setCallbacks(GLFWwindow* native){
    glfwSetFramebufferSizeCallback(native, GLFW_framebuffer_size_callback);
    glfwSetWindowSizeCallback(native, GLFW_window_size_callback);
    glfwSetWindowCloseCallback(native, GLFW_window_close_callback);

    update_size(native);
}

void Window::update_size(GLFWwindow* native){
    int width = 0;
    int height = 0;

    glfwGetWindowSize(native, &width, &height);
    assert( (width >= 0  && height >=0 )&& "OpenGL returned a negative Window size!");
    m_window_size = Window::Size{static_cast<uint32_t>(width),static_cast<uint32_t>(height)};

    glfwGetFramebufferSize(native, &width, &height);
    assert( (width >= 0  && height >=0 )&& "OpenGL returned a negative FrameBuffer size!");
    m_frame_buffer_size = Window::Size{static_cast<uint32_t>(width),static_cast<uint32_t>(height)};
}

Window::Size Window::getWindowSize() const{
    return m_window_size;
}

Window::Size Window::getFrameBufferSize()const{
    return m_frame_buffer_size;
}


void Window::GLFW_framebuffer_size_callback( [[maybe_unused]] GLFWwindow* window, int width, int height){
    WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));


    if (context) {
        Window& instance = context->m_window;
        
        instance.m_frame_buffer_size.width = width;
        instance.m_frame_buffer_size.height = height;
    }
}
void Window::GLFW_window_size_callback(GLFWwindow* window, int width, int height) {
    WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    
    if (context) {
        Window& instance = context->m_window;
        
        instance.m_window_size.width = width;
        instance.m_window_size.height = height;
    }
}
void Window::GLFW_window_close_callback([[maybe_unused]] GLFWwindow* window){
    std::cout<<"Window being closed\n";
}