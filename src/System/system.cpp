#include "System/system.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>


void init_glfw(void (*error_function)(int, const char*) ){
    glfwSetErrorCallback(error_function);

    /* Initialize the library */
    if (glfwInit()!=GLFW_TRUE){
        fprintf(stderr, "glfw did not initialise properly\n");
        exit(1);
    } 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void init_glew(){
    GLenum GLsucess = glewInit();
    if(GLsucess != GLEW_OK){
        fprintf(stderr, "GLEW Initialization Error: %s\n", glewGetErrorString(GLsucess));
        exit(1);
    }
    glEnable(GL_DEPTH_TEST);  // Should this be here, it should go to master renderer

    printf("%s\n",glGetString(GL_VERSION));
}


WindowContext::WindowContext(uint32_t width, uint32_t height, const char* title){
    init_glfw(GLFW_error_callback);

    this->m_native = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!m_native){
        fprintf(stderr, "glfw window did not open properly\n");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(m_native);
    glfwSetWindowUserPointer(m_native, this);

    this->m_keyboard.setCallbacks(m_native);
    this->m_mouse.setCallbacks(m_native);
    this->m_window.setCallbacks(m_native);

    init_glew();
}

WindowContext::~WindowContext(){
    if(m_native){
        glfwDestroyWindow(m_native);
    }
    glfwTerminate();
}
    




void WindowContext::startFrame(){
    glfwPollEvents();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_mouse.startFrame();
}
void WindowContext::endFrame(){
    glfwSwapBuffers(m_native);


    m_keyboard.EndFrame();
    m_mouse.endFrame();
}
bool WindowContext::isOpen(){
    return !glfwWindowShouldClose(m_native);
}


void WindowContext::GLFW_error_callback(int /*error*/, const char* description){
    fprintf(stderr, "GLFW Error: %s\n", description);
}
