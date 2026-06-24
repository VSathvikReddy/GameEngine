#include "System/mouse.hpp"
#include "System/system.hpp"
#include <GLFW/glfw3.h>

void Mouse::endFrame() {
    m_PreviousButtons = m_CurrentButtons;
    previous = current;
    m_cursor_border_crossed = false;
    
    m_scrollX = 0.0;
    m_scrollY = 0.0;
}
void Mouse::startFrame(){
    if(m_cursor_border_crossed) {
        previous = current;
    }
}




void Mouse::setCallbacks(GLFWwindow* window) {
    glfwSetMouseButtonCallback(window, Mouse::GLFW_mouse_button_callback);
    glfwSetCursorPosCallback(window, Mouse::GLFW_cursor_position_callback);
    glfwSetScrollCallback(window, Mouse::GLFW_scroll_callback);
    glfwSetCursorEnterCallback(window, Mouse::GLFW_cursor_enter_callback);

    glfwGetCursorPos(window, &current.x, &current.y);
    m_cursor_border_crossed = true;
}


// --- Static GLFW Callback Bridges ---

void Mouse::GLFW_mouse_button_callback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
    Mouse& instance = (static_cast<WindowContext*>(glfwGetWindowUserPointer(window))->m_mouse);
    
    if (button < 0 || button >= 8) return;

    if (action == GLFW_PRESS) {
        instance.m_CurrentButtons.set(button);
    } else if (action == GLFW_RELEASE) {
        instance.m_CurrentButtons.reset(button);
    }
}


void Mouse::GLFW_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    WindowContext* instance = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if(!instance){ return;}

    instance->m_mouse.current.x = xpos;
    instance->m_mouse.current.y = ypos;
}
void Mouse::GLFW_cursor_enter_callback(GLFWwindow* window, int entered) {
    WindowContext* instance = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if(!instance) { return;}

    instance->m_mouse.m_active = (entered == GLFW_TRUE);
    instance->m_mouse.m_cursor_border_crossed = true;
}
void Mouse::GLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    WindowContext* instance = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));

    if(!instance){ return;}

    instance->m_mouse.m_scrollX += xoffset;
    instance->m_mouse.m_scrollY += yoffset;
}