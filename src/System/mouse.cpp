#include "System/mouse.hpp"
#include "System/system.hpp"
#include <GLFW/glfw3.h>

void Mouse::EndFrame() {
    // Sync current buttons to previous for edge detection
    m_PreviousButtons = m_CurrentButtons;
    previous = current;
    
    m_scrollX = 0.0;
    m_scrollY = 0.0;
}





Mouse::Coords Mouse::getPosition() const { return current; }
Mouse::Coords Mouse::getDelta() const { return {current.x-previous.x,current.y - previous.y}; }

double Mouse::getScrollX() const { return m_scrollX; }
double Mouse::getScrollY() const { return m_scrollY; }




bool Mouse::isButtonHeld(Mouse::Button button) const {
    return m_CurrentButtons.test(static_cast<size_t>(button));
}

bool Mouse::isButtonPressed(Mouse::Button button) const {
    size_t b = static_cast<size_t>(button);
    return m_CurrentButtons.test(b) && !m_PreviousButtons.test(b);
}

bool Mouse::isButtonReleased(Mouse::Button button) const {
    size_t b = static_cast<size_t>(button);
    return !m_CurrentButtons.test(b) && m_PreviousButtons.test(b);
}

void Mouse::setCallbacks(GLFWwindow* window) {
    glfwSetMouseButtonCallback(window, Mouse::GLFW_mouse_button_callback);
    glfwSetCursorPosCallback(window, Mouse::GLFW_cursor_position_callback);
    glfwSetScrollCallback(window, Mouse::GLFW_scroll_callback);
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
    Mouse& instance = (static_cast<WindowContext*>(glfwGetWindowUserPointer(window))->m_mouse);

    if (instance.m_firstMouse) {
        instance.previous.x = xpos;
        instance.previous.y = ypos;
        instance.m_firstMouse = false;
    }
    

    instance.current.x = xpos;
    instance.current.y = ypos;
}

void Mouse::GLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Mouse& instance = (static_cast<WindowContext*>(glfwGetWindowUserPointer(window))->m_mouse);
    instance.m_scrollX += xoffset;
    instance.m_scrollY += yoffset;
}