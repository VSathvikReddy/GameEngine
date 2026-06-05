#include "Shader/shader_manager.hpp"
#include "Shader/pre_processor.hpp"
#include "Shader/shader.hpp"
#include <iostream>
#include <cassert>

ShaderManager::ShaderManager() noexcept {
    shader_data.emplace_back(); 
    active_shader_ID = 0; 
}

ShaderManager::~ShaderManager() noexcept{
    for(const auto& x: shader_data){
        glDeleteProgram(x.programID);
    }
    shader_data.clear();
    string_to_shader_ID.clear();
}

void ShaderManager::loadShader(const std::filesystem::path& folder, std::string_view shaderName, std::string_view userDefines) noexcept {
    std::string nameKey(shaderName);
    
    if (string_to_shader_ID.find(nameKey) != string_to_shader_ID.end()) {
        return;
    }

    std::filesystem::path vertFilePath = folder / (nameKey + ".vert");
    std::filesystem::path fragFilePath = folder / (nameKey + ".frag");

    std::string vertPathStr = vertFilePath.string();
    std::string fragPathStr = fragFilePath.string();

#ifndef NDEBUG
    bool vertExists = std::filesystem::exists(vertFilePath);
    bool fragExists = std::filesystem::exists(fragFilePath);

    if (!vertExists) {
        std::cerr << "[Shader Error] Missing Vertex Shader File at: " << vertPathStr << "\n";
    }
    if (!fragExists) {
        std::cerr << "[Shader Error] Missing Fragment Shader File at: " << fragPathStr << "\n";
    }

    assert(vertExists && "CRITICAL: Vertex shader path does not exist on disk!");
    assert(fragExists && "CRITICAL: Fragment shader path does not exist on disk!");
#endif

    std::string vertexShaderSource = ShaderPreProcessor::Process(vertPathStr.c_str(), std::string(userDefines));
    std::string fragmentShaderSource = ShaderPreProcessor::Process(fragPathStr.c_str(), std::string(userDefines));

    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        std::cerr << "ERROR::SHADER::MANAGER::PREPROCESS_FAILED for: " << nameKey << "\n";
        return;
    }

#ifndef NDEBUG
    char infoLog[512];
    char* infoLogPtr = infoLog;
#else
    char* infoLogPtr = nullptr;
#endif

    unsigned int vertexShaderID = ShaderManager::createShader(GL_VERTEX_SHADER, vertexShaderSource.c_str(), infoLogPtr);
    unsigned int fragmentShaderID = ShaderManager::createShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str(), infoLogPtr);

    if (vertexShaderID == 0 || fragmentShaderID == 0) {
        if (vertexShaderID != 0) glDeleteShader(vertexShaderID);
        if (fragmentShaderID != 0) glDeleteShader(fragmentShaderID);
        
        assert(false && "CRITICAL: Compilation failed for one or more shader stages! Inspect console outputs.");
        return; 
    }
    ShaderGPUID ID = ShaderManager::linkShaders(vertexShaderID, fragmentShaderID, infoLogPtr);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    assert(ID != 0 && "Unable to link the Shaders");
    if (ID == 0) return;

    ShaderID newEngineID = static_cast<ShaderID>(shader_data.size());
    shader_data.push_back({ID});
    string_to_shader_ID[nameKey] = newEngineID;
}

unsigned int ShaderManager::createShader(GLenum type, const char* ShaderSource, char* infoLog) {
    unsigned int Shader = glCreateShader(type);
    glShaderSource(Shader, 1, &ShaderSource, NULL);
    glCompileShader(Shader);

    int success;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        if (infoLog) {
            glGetShaderInfoLog(Shader, 512, NULL, infoLog);
            const char* shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "UNKNOWN";
            printf("ERROR::SHADER::%s::COMPILATION_FAILED\n%s\n", shaderType, infoLog);
        }

        glDeleteShader(Shader); 
        return 0;               
    }

    return Shader;
}

ShaderGPUID ShaderManager::linkShaders(unsigned int vertexShader, unsigned int fragmentShader, char* infoLog) {
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        if (infoLog) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        }

        glDeleteProgram(shaderProgram); 
        return 0;
    }
    return shaderProgram;
}

// This is the only way to create a shader, and it will throw error if I release an invalid shader
Shader ShaderManager::getShader(std::string_view shaderName){
    return Shader(*this,string_to_shader_ID.at(std::string(shaderName)));
}

void ShaderManager::clear(){
    glUseProgram(0);
    active_shader_ID = 0;
}
void ShaderManager::use(ShaderID ID){
    assert(ID < shader_data.size() && "Out of bounds ShaderID execution request!");
    if(ID == active_shader_ID ) return;

    glUseProgram(shader_data.at(ID).programID);
    active_shader_ID = ID;
}


Uniform ShaderManager::getUniform(ShaderID ID, std::string_view name){
    std::string key(name);
    auto& target_data = this->shader_data.at(ID);
    auto& map = target_data.uniform_map;
    
    auto it = map.find(key);
    if (it != map.end()){
        return it->second;
    }
    Uniform uni(target_data.programID,key.c_str());
    map.emplace(key,uni);
    return uni;
}
