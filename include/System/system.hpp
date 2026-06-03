#pragma once

#include "System/mouse.hpp"
#include "System/window.hpp"
#include "System/keyboard_keys.hpp"

#include "System/clock.hpp"

class GLFWwindow;





class WindowContext{
public:
    WindowContext(uint32_t width = 0, uint32_t height = 0, const char* title = "Default Name");
    ~WindowContext();

    WindowContext(const WindowContext& other) = delete;
    WindowContext(WindowContext&& other) = delete;
    WindowContext& operator=(const WindowContext& other) = delete;
    WindowContext& operator=(WindowContext&& other) = delete;

    friend class Keyboard;
    friend class Mouse;
    friend class Window;


    
    const Keyboard& getKeyboard() const;
    const Mouse& getMouse() const;
    const Window& getWindow() const;

    const TimeD& getDt() const;
    const Clock& getClock() const;


    void startFrame();
    void endFrame();
    bool isOpen();
private:
    Keyboard m_keyboard;
    Mouse m_mouse;
    Window m_window;

    Clock clk;
    TimeD dt;

    GLFWwindow* m_native = nullptr;

    static void GLFW_error_callback(int error, const char* description);
};