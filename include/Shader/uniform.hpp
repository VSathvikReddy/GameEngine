#pragma once

#include <glm/glm.hpp>
// #include "Geometry/vector3.hpp"
#include <GL/glew.h>


struct Uniform{
    const unsigned int ID=0;

    Uniform(unsigned int shader_ID, const char* name);
    Uniform(unsigned int location_ID);

    template<typename T>
    void setValue(const T& a);
    template<typename T>
    void setValue(const T& a, const T& b);
    template<typename T>
    void setValue(const T& a, const T& b, const T& c);
    template<typename T>
    void setValue(const T* a);

};

inline Uniform::Uniform(unsigned int shader_ID, const char* name):
    ID(glGetUniformLocation(shader_ID, name)){
}
inline Uniform::Uniform(unsigned int location_ID):
        ID(location_ID){        
}


template<> 
inline void Uniform::setValue<float>(const float& a) {
    glUniform1f(ID, a);
}

template<> 
inline void Uniform::setValue<float>(const float& a, const float& b) {
    glUniform2f(ID, a, b);
}

template<> 
inline void Uniform::setValue<float>(const float& a, const float& b, const float& c) {
    glUniform3f(ID, a, b, c);
}

// =========================================================================
// SIGNED INTEGERS SPECIALIZATIONS (1D, 2D, 3D)
// =========================================================================

template<> 
inline void Uniform::setValue<int>(const int& a) {
    glUniform1i(ID, a);
}

template<> 
inline void Uniform::setValue<int>(const int& a, const int& b) {
    glUniform2i(ID, a, b);
}

template<> 
inline void Uniform::setValue<int>(const int& a, const int& b, const int& c) {
    glUniform3i(ID, a, b, c);
}

// =========================================================================
// UNSIGNED INTEGERS SPECIALIZATIONS
// =========================================================================

template<> 
inline void Uniform::setValue<unsigned int>(const unsigned int& a) {
    glUniform1ui(ID, a);
}

// =========================================================================
// BOOLEANS SPECIALIZATION
// =========================================================================

template<> 
inline void Uniform::setValue<bool>(const bool& a) {
    glUniform1i(ID, static_cast<int>(a));
}

// =========================================================================
// GLM MATRICES SPECIALIZATION
// =========================================================================

template<>
inline void Uniform::setValue<glm::mat4>(const glm::mat4& matrix) {
    glUniformMatrix4fv(ID, 1, GL_FALSE, &matrix[0][0]);
}


template<> 
inline void Uniform::setValue<float>(const float* a) {
    glUniform2fv(ID, 1, a); 
}