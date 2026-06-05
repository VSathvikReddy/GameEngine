#ifndef SHADER_H
#define SHADER_H

#include <string_view>
#include "Shader/shader_manager.hpp"

class ShaderManager;

using ShaderID = uint32_t;
using ShaderGPUID = uint32_t;
class Shader{
public:
    Shader() = delete;

    void use() const noexcept { manager.use(ID); }

    // Texture is setUnifor("Name",slot);
    template<typename T>
    void setUniform(std::string_view name, const T& a) const noexcept;
    template<typename T>
    void setUniformUnsafe(std::string_view name, const T& a) const noexcept;

    Shader(const Shader&) noexcept = default;
    Shader& operator=(const Shader&) noexcept = default;

private:
    friend class ShaderManager;
    ShaderManager& manager;
    ShaderID ID;

    Shader(ShaderManager& manager,ShaderID ID):manager(manager),ID(ID){}
};

template<typename T>
inline void Shader::setUniform(std::string_view name, const T& a) const noexcept{
    manager.setUniform(ID,name,a);
}

template<typename T>
inline void Shader::setUniformUnsafe(std::string_view name, const T& a) const noexcept{
    manager.setUniformUnsafe(ID,name,a);
}

#endif