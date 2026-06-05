#pragma once

#include "Shader/uniform.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <string>

class Shader;
using ShaderID = uint32_t;
using ShaderGPUID = uint32_t;

class ShaderManager {
public:
    ShaderManager() noexcept;
    ~ShaderManager() noexcept;

    // Copyin breaks the reference system
    ShaderManager(const ShaderManager& other) = delete;
    ShaderManager& operator=(const ShaderManager& other) = delete;

    void loadShader(const std::filesystem::path& folder, std::string_view shaderName, std::string_view userDefines = "") noexcept;

    [[nodiscard]] Shader getShader(std::string_view shaderName);

    void clear();
private:
    friend class Shader;

    struct ShaderData {
        ShaderGPUID programID = 0;
        std::unordered_map<std::string, Uniform> uniform_map;
    };
    
    std::unordered_map<std::string, ShaderID> string_to_shader_ID;
    std::vector<ShaderData> shader_data;
    ShaderID active_shader_ID = 0;


    void use(ShaderID ID);
    
    template<typename T> void setUniform(ShaderID ID, std::string_view name, const T& a) noexcept;
    template<typename T> void setUniformUnsafe(ShaderID ID, std::string_view name, const T& a) noexcept;

    Uniform getUniform(ShaderID ID, std::string_view name);

    static unsigned int createShader(GLenum type, const char* shaderSource, char* infoLog);
    static unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader, char* infoLog);
};






template<typename T>
inline void ShaderManager::setUniform(ShaderID ID, std::string_view name, const T& a) noexcept{
    use(ID);
    getUniform(ID,name).setValue(a);
}
template<typename T> 
inline void ShaderManager::setUniformUnsafe(ShaderID ID, std::string_view name, const T& a) noexcept{
    getUniform(ID,name).setValue(a);
}