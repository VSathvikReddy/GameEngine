#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define DEFAULT_VERTEX_SHADER "shaders/basic.vert"
#define DEFAULT_FRAGMENT_SHADER "shaders/basic.frag"
#define LIGHT_FRAGMENT_SHADER "shaders/light.frag"

#include "Shader/uniform.hpp"

class Shader{
private:
    unsigned int ID = 0;
    std::unordered_map<std::string,Uniform> uniform_map;
    
    static unsigned int createShader(GLenum type, const char* ShaderSource, char* infoLog);
    static unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader, char* infoLog);

public:
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const std::string& userDefines = "");
    ~Shader();
    
    Shader(const Shader&) = delete;             // no copying
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;      // move support


    void use();
    unsigned int getID() const;

    template<typename T>
    void setUniform(const char* name,const T& value);

    //s plural
    void setUniforms() {}
    template<typename T, typename... Args>     // Variadic template to unpack name-value pairs
    void setUniforms(const char* name, const T& value, Args... args);
};

template<typename T, typename... Args>
void Shader::setUniforms(const char* name,const T& value, Args... args){
    this->setUniform(name, value);
    setUniforms(args...);
}

template<typename T>
void Shader::setUniform(const char* name,const T& value){
    auto itr = uniform_map.find(name);
    if(itr == uniform_map.end()){        
        auto result = uniform_map.emplace(name,Uniform(ID,name));
        result.first->second.setValue(value);
    }else{
        itr->second.setValue(value);
    }   
}

#endif