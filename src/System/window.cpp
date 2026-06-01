#include "System/window.hpp"
#include "System/system.hpp"
#include <GLFW/glfw3.h>

#include <iostream>

void Window::setCallbacks(GLFWwindow* native){
    glfwSetFramebufferSizeCallback(native, GLFW_framebuffer_size_callback);
    glfwSetWindowSizeCallback(native, GLFW_window_size_callback);
    glfwSetWindowCloseCallback(native, GLFW_window_close_callback);

    update_size(native);
}

void Window::update_size(GLFWwindow* native){
    int winWidth = 0;
    int winHeight = 0;
    glfwGetWindowSize(native, &winWidth, &winHeight);

    // Since this is a normal member function, you can access m_cache_size directly!
    m_cache_size.width = winWidth;
    m_cache_size.height = winHeight;
}

Window::Size Window::getSize() const{
    return m_cache_size;
}


void Window::GLFW_framebuffer_size_callback( [[maybe_unused]] GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void Window::GLFW_window_size_callback(GLFWwindow* window, int width, int height) {
    WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (context) {
        Window& instance = context->m_window;
        
        instance.m_cache_size.width = width;
        instance.m_cache_size.height = height;
    }
}
void Window::GLFW_window_close_callback([[maybe_unused]] GLFWwindow* window){
    std::cout<<"Window being closed\n";
}