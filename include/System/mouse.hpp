#pragma once
#include <bitset>
#include <cstdint>

struct GLFWwindow;

class Mouse {
public:
    // 1. Fully define nested types here so they are complete
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

    void EndFrame();
    void setCallbacks(GLFWwindow* window);

    // Button states
    bool isButtonHeld(Button button) const;
    bool isButtonPressed(Button button) const;
    bool isButtonReleased(Button button) const;
    
    Coords getPosition() const;
    Coords getDelta() const;
    
    double getScrollX() const;
    double getScrollY() const;

private:
    std::bitset<8> m_CurrentButtons;
    std::bitset<8> m_PreviousButtons;

    Coords current;
    Coords previous; 

    double m_scrollX = 0.0, m_scrollY = 0.0;
    bool m_firstMouse = true;

    static void GLFW_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void GLFW_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void GLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};