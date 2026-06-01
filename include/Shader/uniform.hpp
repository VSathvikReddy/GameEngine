#pragma once

#include <glm/glm.hpp>
// #include "Geometry/vector3.hpp"
#include <GL/glew.h>


struct Uniform{
    const unsigned int ID=0;

    Uniform(unsigned int shader_ID, const char* name):
        ID(glGetUniformLocation(shader_ID, name)){
    }
    Uniform(unsigned int location_ID):
            ID(location_ID){        
    }

    template<typename T>
    void setValue(const T& value);
};



template<>
inline void Uniform::setValue(const bool& value){
    glUniform1i(ID, static_cast<int>(value)); 
}
template<>
inline void Uniform::setValue(const int& value){
    glUniform1i(ID, value); 
}

template<>
inline void Uniform::setValue(const float& value){
    glUniform1f(ID, value); 
}
template<>
inline void Uniform::setValue(const glm::mat4& value){
    //                Loc,count,No Trans,Value 
    glUniformMatrix4fv(ID, 1, GL_FALSE, &value[0][0]); 
}
// template<>
// inline void Uniform::setValue(const Vec3f& value){
//     glUniform3f(ID, value[0], value[1], value[2]);
// }
