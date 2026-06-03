#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>


#define DEFAULT_VERTEX_SHADER "shaders/basic.vert"
#define DEFAULT_FRAGMENT_SHADER "shaders/basic.frag"
#define LIGHT_FRAGMENT_SHADER "shaders/light.frag"

#include "Shader/uniform.hpp"

class Shader{
public:
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const std::string& userDefines = "");
    ~Shader();

    void use() const;
    unsigned int getID() const;

    template<typename T>
    void setUniform(const char* name,const T& a);
    template<typename T>
    void setUniform(const char* name,const T& a, const T& b);
    template<typename T>
    void setUniform(const char* name,const T& a, const T& b, const T& c);
    template<typename T>
    void setUniform(const char* name,const T* a);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

private:
    unsigned int ID = 0;
    std::unordered_map<std::string,Uniform> uniform_map;
    
    static unsigned int createShader(GLenum type, const char* ShaderSource, char* infoLog);
    static unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader, char* infoLog);

    Uniform getUniform(const char* name);
};

template<typename T>
void Shader::setUniform(const char* name, const T& a) {
    getUniform(name).setValue(a);
}

template<typename T>
void Shader::setUniform(const char* name, const T& a, const T& b) {
    getUniform(name).setValue(a, b);
}

template<typename T>
void Shader::setUniform(const char* name, const T& a, const T& b, const T& c) {
    getUniform(name).setValue(a, b, c);
}

template<typename T>
void Shader::setUniform(const char* name, const T* a) {
    getUniform(name).setValue(a);
}

#endif