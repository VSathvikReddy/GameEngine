#pragma once

#include "Render/Shader/uniform.hpp"

#include <filesystem>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <iostream>

#include "Render/gl_alias.hpp"

struct Ubo{
    const UboID bufferID;
    const GLint size;
    const uint32_t globalSlot;
};

class ShaderManager {
public:
    ShaderManager() noexcept;
    ~ShaderManager() noexcept;

    ShaderManager(const ShaderManager& other) = delete;
    ShaderManager& operator=(const ShaderManager& other) = delete;


    [[nodiscard]] ShaderID loadShader(const std::filesystem::path& folder, const std::string& shaderName, std::string_view userDefines = "") noexcept;


    [[nodiscard]] ShaderID getShader(const std::string& shaderName) const;
    void clearActiveShader();
    void activateShader(ShaderID ID);
    

    [[nodiscard]] Ubo setUBO(const std::string& name);


    Uniform getUniform(ShaderID ID, const std::string& name); // Please dont store and use

private:
    struct ShaderData {
        ShaderGPUID programID = 0;
        std::unordered_map<std::string, Uniform> uniform_map;
    };
    
    std::unordered_map<std::string, ShaderID> string_to_shader_ID;
    std::vector<ShaderData> shader_data;
    
    ShaderID active_shader_ID;

    std::unordered_map<std::string, Ubo> registered_ubos;
    uint32_t available_global_ubo_slot = 0;

    
};


inline ShaderID ShaderManager::getShader(const std::string& shaderName) const{
    auto it = string_to_shader_ID.find(shaderName);
    if (it != string_to_shader_ID.end()) {
        return it->second;
    }
#ifndef NDEBUG
    std::cerr << "[Shader Error] Attempted to get unregistered or failed shader: '" << shaderName << "'. Returning ERROR_SHADER.\n";
#endif
    return ERROR_SHADER;
}


inline void ShaderManager::clearActiveShader(){
    glUseProgram(0);
    active_shader_ID = NO_SHADER;
}

inline void ShaderManager::activateShader(ShaderID ID){
    assert(ID < shader_data.size() && "Out of bounds ShaderID execution request!");
    assert(ID != ERROR_SHADER && "ERROR_SHADER being used");
    if(ID == active_shader_ID ) return;

    glUseProgram(shader_data.at(ID).programID);
    active_shader_ID = ID;
}


