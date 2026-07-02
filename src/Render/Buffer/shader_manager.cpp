#include "Render/Buffer/shader_manager.hpp"

ShaderManager::ShaderManager(){
    m_shader_gpu_data.emplace_back(0, string_hash_map<Uniform>{});
    m_shader_ids["__No_Shader__"] = NO_SHADER;
}

ShaderManager::~ShaderManager(){
    for(const auto& shader_data: m_shader_gpu_data){
        glDeleteProgram(shader_data.programID);
    }
}

ShaderID ShaderManager::loadShader(const std::string& shader_name, const std::filesystem::path& folder){
    
}