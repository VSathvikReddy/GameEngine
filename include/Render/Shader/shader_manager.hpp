#pragma once

#include "Render/Shader/uniform.hpp"

#include <filesystem>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <iostream>

using ShaderID = uint32_t;
using ShaderGPUID = uint32_t;
constexpr ShaderID ERROR_SHADER = 0;

class ShaderManager {
public:
    ShaderManager() noexcept;
    ~ShaderManager() noexcept;

    ShaderManager(const ShaderManager& other) = delete;
    ShaderManager& operator=(const ShaderManager& other) = delete;


    [[nodiscard]] ShaderID loadShader(const std::filesystem::path& folder, const std::string& shaderName, std::string_view userDefines = "") noexcept;


    [[nodiscard]] ShaderID getShader(const std::string& shaderName) const;
    void clear();
    void use(ShaderID ID);
    

    [[nodiscard]] uint32_t setUBOs(const std::string& name);


    template<typename T> void setUniform(ShaderID ID, const std::string&  name, const T& a) noexcept;
    template<typename T> void setUniformUnsafe(ShaderID ID, const std::string&  name, const T& a) noexcept;

private:
    struct ShaderData {
        ShaderGPUID programID = 0;
        std::unordered_map<std::string, Uniform> uniform_map;
    };
    
    std::unordered_map<std::string, ShaderID> string_to_shader_ID;
    std::vector<ShaderData> shader_data;
    
    ShaderID active_shader_ID;

    std::unordered_map<std::string, uint32_t> registered_ubos;
    uint32_t available_ubo_slot = 0; // Needs to be called after loading all shaders

    Uniform getUniform(ShaderID ID, const std::string& name);
};


template<typename T>
inline void ShaderManager::setUniform(ShaderID ID, const std::string& name, const T& a) noexcept {
    use(ID);
    this->getUniform(ID, name).setValue(a);
}

template<typename T> 
inline void ShaderManager::setUniformUnsafe(ShaderID ID, const std::string&  name, const T& a) noexcept {
    this->getUniform(ID, name).setValue(a);
}



inline void ShaderManager::clear(){
    glUseProgram(0);
    active_shader_ID = 0;
}

inline void ShaderManager::use(ShaderID ID){
    assert(ID < shader_data.size() && "Out of bounds ShaderID execution request!");
    assert(ID != ERROR_SHADER && "ERROR_SHADER being used");
    if(ID == active_shader_ID ) return;

    glUseProgram(shader_data.at(ID).programID);
    active_shader_ID = ID;
}


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