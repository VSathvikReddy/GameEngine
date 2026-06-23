#include "Render/Shader/shader_manager.hpp"

#include <iostream>
#include <cassert>

ShaderManager::ShaderManager() noexcept {
    shader_data.emplace_back(); 
    active_shader_ID = ERROR_SHADER; 
}

ShaderManager::~ShaderManager() noexcept{
    for(const auto& x: shader_data){
        glDeleteProgram(x.programID);
    }
}


ShaderID ShaderManager::getShader(const std::string& shaderName) const{
    auto it = string_to_shader_ID.find(shaderName);
    if (it != string_to_shader_ID.end()) {
        return it->second;
    }
#ifndef NDEBUG
    std::cerr << "[Shader Error] Attempted to get unregistered or failed shader: '" << shaderName << "'. Returning ERROR_SHADER.\n";
#endif
    return ERROR_SHADER;
}

void ShaderManager::clear(){
    glUseProgram(0);
    active_shader_ID = 0;
}

void ShaderManager::use(ShaderID ID){
    assert(ID < shader_data.size() && "Out of bounds ShaderID execution request!");
    assert(ID != ERROR_SHADER && "ERROR_SHADER being used");
    if(ID == active_shader_ID ) return;

    glUseProgram(shader_data.at(ID).programID);
    active_shader_ID = ID;
}




uint32_t ShaderManager::setUBOs(const std::string& name){
    auto it = registered_ubos.find(name);
    if (it != registered_ubos.end()) {
        return it->second; 
    }

    if (available_ubo_slot >= 14) {
        throw std::runtime_error("Engine Error: Exhausted maximum hardware UBO slots when binding");
    }

    for (const auto& itr : shader_data) {
        unsigned int idx = glGetUniformBlockIndex(itr.programID, name.c_str());
        if (idx != GL_INVALID_INDEX) {
            glUniformBlockBinding(itr.programID, idx, available_ubo_slot);
        }
    }
    registered_ubos[name] = available_ubo_slot;
    return available_ubo_slot++;
}



Uniform ShaderManager::getUniform(ShaderID ID, const std::string& name){
    auto& target_data = this->shader_data.at(ID);
    auto& map = target_data.uniform_map;
    
    auto it = map.find(name);
    if (it != map.end()){
        return it->second;
    }
    Uniform uni(target_data.programID,name.c_str());
    map.emplace(name,uni);
    return uni;
}
