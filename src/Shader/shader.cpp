#include <string>
#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader/shader.hpp"
#include "Shader/pre_processor.hpp"

unsigned int Shader::createShader(GLenum type, const char* ShaderSource, char* infoLog){
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int Shader = glCreateShader(type);
    glShaderSource(Shader, 1, &ShaderSource, NULL);
    glCompileShader(Shader);
    // check for shader compile errors

    int success;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(Shader, 512, NULL, infoLog);
        const char* shaderType =(type == GL_VERTEX_SHADER) ? "VERTEX" :(type == GL_FRAGMENT_SHADER) ? "FRAGMENT" :"UNKNOWN";
        printf("ERROR::SHADER::%s::COMPILATION_FAILED\n%s\n", shaderType, infoLog);

        glDeleteShader(Shader); // Purge the broken object from the GPU
        return 0;               // Return 0 indicating failure
    }

    return Shader;
}
unsigned int Shader::linkShaders(unsigned int vertexShader, unsigned int fragmentShader, char* infoLog){
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);

        glDeleteProgram(shaderProgram); // Purge the broken program layout
        return 0;
    }
    return shaderProgram;
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const std::string& userDefines){
    std::string vertexShaderSource = ShaderPreProcessor::Process(vertexShaderPath, userDefines);
    std::string fragmentShaderSource = ShaderPreProcessor::Process(fragmentShaderPath, userDefines);
    if(vertexShaderSource.empty() || fragmentShaderSource.empty()){
        return; //ID is set to 0
    }
    char infoLog[512];
    unsigned int vertexShaderID = Shader::createShader(GL_VERTEX_SHADER, vertexShaderSource.c_str(), infoLog);
    unsigned int fragmentShaderID = Shader::createShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str(), infoLog);

    if (vertexShaderID == 0 || fragmentShaderID == 0) {
        if (vertexShaderID != 0) glDeleteShader(vertexShaderID);
        if (fragmentShaderID != 0) glDeleteShader(fragmentShaderID);
        this->ID = 0;
        return;
    }

    this->ID = Shader::linkShaders(vertexShaderID, fragmentShaderID, infoLog);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}





Uniform Shader::getUniform(const char* name){
    auto itr = uniform_map.find(name);
    if(itr == uniform_map.end()){        
        auto result = uniform_map.emplace(name,Uniform(ID,name));
        return result.first->second;
    }else{
        return itr->second;
    }
}




Shader::Shader(Shader&& other) noexcept{
    ID = other.ID;
    other.ID = 0;
    this->uniform_map = std::move(other.uniform_map);
}
Shader& Shader::operator=(Shader&& other) noexcept{
    if(this->ID!=0){
        glDeleteProgram(this->ID);
    }
    this->ID = other.ID;
    other.ID = 0; 
    this->uniform_map = std::move(other.uniform_map);  

    return *this;
}
Shader::~Shader(){
    if(this->ID!=0){
        glDeleteProgram(this->ID);
    }
}

void Shader::use() const{
    if(ID!=0) glUseProgram(ID);
}
unsigned int Shader::getID() const{
    return ID;
}



