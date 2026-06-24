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
    for(const auto& [name, ubo] : registered_ubos){
        glDeleteBuffers(1, &ubo.bufferID);
    }
}


inline Ubo bindUbo(ShaderGPUID targetProgramID, uint32_t targetBlockIndex, uint32_t assignedSlot){
    GLint blockSize = 0;
    glGetActiveUniformBlockiv(targetProgramID, targetBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

    UboID bufferID = 0;
    glGenBuffers(1, &bufferID);
    
    glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_DYNAMIC_DRAW); // This is to be taught, what type
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, assignedSlot, bufferID, 0, blockSize);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return Ubo{bufferID,blockSize,assignedSlot};
}

Ubo ShaderManager::setUBO(const std::string& name){
    assert(registered_ubos.find(name) == registered_ubos.end() && "Same UBO being register again");
    assert(available_global_ubo_slot < 14 && "Engine Error: Exhausted maximum hardware UBO slots when binding" );
    assert(!shader_data.empty() && "No shaders loaded yet, so unable to querry for the ubo details");

    ShaderGPUID targetProgramID = 0;
    uint32_t targetBlockIndex = GL_INVALID_INDEX;

    for (auto itr = shader_data.begin() + 1; itr != shader_data.end(); itr++) {
        unsigned int idx = glGetUniformBlockIndex(itr->programID, name.c_str());
        if (idx != GL_INVALID_INDEX) {
            glUniformBlockBinding(itr->programID, idx, available_global_ubo_slot);
            if (targetBlockIndex == GL_INVALID_INDEX) {
                targetProgramID = itr->programID;
                targetBlockIndex = idx;
            }
        }
    }
    assert(targetBlockIndex != GL_INVALID_INDEX && "Engine Error: The requested UBO block name was not found inside ANY currently loaded shaders!");
    Ubo out = bindUbo(targetProgramID, targetBlockIndex, available_global_ubo_slot++);
    registered_ubos.emplace(name,out);

    return out;
}



Uniform ShaderManager::getUniform(ShaderID ID, const std::string& name){
    assert(ID == active_shader_ID && "Uniform being retieved without it being the active Shader program");
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
