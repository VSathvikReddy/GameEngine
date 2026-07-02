#pragma once

#include "Render/Buffer/gl_alias.hpp"
#include "Render/Buffer/uniform.hpp"
#include "Core/transparent_hash_table.hpp"

#include <filesystem>
#include <vector>

class ShaderManager{
public:
    ShaderManager();
    ~ShaderManager();

    ShaderID loadShader(const std::string& shader_name, const std::filesystem::path& folder);
    ShaderID getShader(std::string_view shader_name) const;
    Uniform getUniform(ShaderID shader_id, std::string_view uniform_name); // Please make the shader active and only then set values

    void activateShader(ShaderID shader_id);
    void clearActiveShader();

private:
    struct ShaderData{
        ShaderGPUID programID;
        string_hash_map<Uniform> uniform_map;
    };
    std::vector<ShaderData> m_shader_gpu_data;
    string_hash_map<ShaderID> m_shader_ids;
    ShaderID m_active_shader = NO_SHADER;

};