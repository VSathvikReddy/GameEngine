#include "System/window.hpp"
#include "System/system.hpp"
#include <GLFW/glfw3.h>

#include <iostream>

Window::Window(const WindowProperties& properties):m_properties(properties){

}


void Window::setCallbacks(GLFWwindow* native){
    glfwSetFramebufferSizeCallback(native, GLFW_framebuffer_size_callback);
}

const WindowProperties& Window::getProperties() const{
    return m_properties;
}


void Window::GLFW_framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if(context){
        Window& instance = context->m_window;
        instance.m_properties.width = width;
        instance.m_properties.height = height;
        std::cout<<instance.m_properties.height<<' '<<instance.m_properties.width<<'\n';
    }
}

void Window::GLFW_window_close_callback(GLFWwindow* window){
    std::cout<<"Window being closed\n";
}
