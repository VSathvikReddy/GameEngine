#pragma once

#include "System/mouse.hpp"
#include "System/window.hpp"
#include "System/keyboard.hpp"

#include "System/clock.hpp"

class GLFWwindow;





class WindowContext{
public:
    WindowContext(const WindowProperties& properties);
    ~WindowContext();

    friend class Keyboard;
    friend class Mouse;
    friend class Window;


    
    const Keyboard& getKeyboard() const;
    const Mouse& getMouse() const;
    const Window& getWindow() const;

    const TimeD& getDt() const;
    const Clock& getClock() const;


    void pollEvents();
private:
    Keyboard m_keyboard;
    Mouse m_mouse;
    Window m_window;

    Clock clk;
    TimeD dt;

    GLFWwindow* m_native = nullptr;

    static void GLFW_error_callback(int error, const char* description);

};
