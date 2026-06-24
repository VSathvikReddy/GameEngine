#pragma once
#include <bitset>
#include <cstdint>

struct GLFWwindow;

// 0,0 is top left. GLFW notation.
class Mouse {
public:
    struct Coords {
        double x = 0.0;
        double y = 0.0;
    };

    enum class Button : uint8_t {
        Button1 = 0, Button2 = 1, Button3 = 2, Button4 = 3,
        Button5 = 4, Button6 = 5, Button7 = 6, Button8 = 7,
        Left = Button1, Right = Button2, Middle = Button3
    };

    Mouse() = default;

    void endFrame();
    void startFrame();
    void setCallbacks(GLFWwindow* window);

    bool isButtonHeld(Button button) const;
    bool isButtonPressed(Button button) const;
    bool isButtonReleased(Button button) const;
    
    Coords getPosition() const;
    Coords getDelta() const;
    bool isActive() const;
    
    double getScrollX() const;
    double getScrollY() const;

private:
    std::bitset<8> m_CurrentButtons;
    std::bitset<8> m_PreviousButtons;

    Coords current;
    Coords previous; 
    bool m_active = false;
    bool m_cursor_border_crossed = false;

    double m_scrollX = 0.0, m_scrollY = 0.0;

    static void GLFW_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void GLFW_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void GLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLFW_cursor_enter_callback(GLFWwindow* window, int entered);
};












inline Mouse::Coords Mouse::getPosition() const { return current; }
inline Mouse::Coords Mouse::getDelta() const { return {current.x-previous.x,current.y - previous.y}; }

inline double Mouse::getScrollX() const { return m_scrollX; }
inline double Mouse::getScrollY() const { return m_scrollY; }




inline bool Mouse::isButtonHeld(Mouse::Button button) const {
    return m_CurrentButtons.test(static_cast<size_t>(button));
}

inline bool Mouse::isButtonPressed(Mouse::Button button) const {
    size_t b = static_cast<size_t>(button);
    return m_CurrentButtons.test(b) && !m_PreviousButtons.test(b);
}

inline bool Mouse::isButtonReleased(Mouse::Button button) const {
    size_t b = static_cast<size_t>(button);
    return !m_CurrentButtons.test(b) && m_PreviousButtons.test(b);
}