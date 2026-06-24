#pragma once

#include <glm/glm.hpp>
#include "Geometry/vector3.hpp"
#include "Geometry/vector2.hpp"
#include "Geometry/vector4.hpp"
#include <GL/glew.h>

// std::span
// template<typename T>
// struct ArrayView {
//     const T* data;
//     size_t size;

//     // Automatic constructor helper for standard vectors
//     ArrayView(const std::vector<T>& vec) : data(vec.data()), size(vec.size()) {}

//     // Automatic constructor helper for fixed C-style arrays
//     template<size_t N>
//     ArrayView(const T(&arr)[N]) : data(arr), size(N) {}
// };

struct Uniform{
    const unsigned int ID=0;


    Uniform(unsigned int shader_ID, const char* name);
    Uniform(unsigned int location_ID);

    template<typename T>
    void setValue(const T& a);

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
inline void Uniform::setValue<int>(const int& a) {
    glUniform1i(ID, a);
}


template<> 
inline void Uniform::setValue<unsigned int>(const unsigned int& a) {
    glUniform1ui(ID, a);
}

template<> 
inline void Uniform::setValue<bool>(const bool& a) {
    glUniform1i(ID, static_cast<int>(a));
}


template<>
inline void Uniform::setValue<Vec2f>(const Vec2f& v) {
    glUniform2f(ID, v.x, v.y);
}

template<>
inline void Uniform::setValue<Vec3f>(const Vec3f& v) {
    glUniform3f(ID, v.x, v.y, v.z);
}

template<>
inline void Uniform::setValue<Vec4f>(const Vec4f& v) {
    glUniform4f(ID, v.x, v.y, v.z, v.w);
}


template<>
inline void Uniform::setValue<Vec2i>(const Vec2i& v) {
    glUniform2i(ID, v.x, v.y);
}

template<>
inline void Uniform::setValue<Vec3i>(const Vec3i& v) {
    glUniform3i(ID, v.x, v.y, v.z);
}

template<>
inline void Uniform::setValue<Vec4i>(const Vec4i& v) {
    glUniform4i(ID, v.x, v.y, v.z, v.w);
}


template<>
inline void Uniform::setValue<glm::mat3>(const glm::mat3& matrix) {
    // GL_FALSE means "do not transpose the matrix". GLM matrices are already column-major, 
    // which matches exactly what OpenGL natively expects!
    glUniformMatrix3fv(ID, 1, GL_FALSE, &matrix[0][0]);
}

template<>
inline void Uniform::setValue<glm::mat4>(const glm::mat4& matrix) {
    glUniformMatrix4fv(ID, 1, GL_FALSE, &matrix[0][0]);
}