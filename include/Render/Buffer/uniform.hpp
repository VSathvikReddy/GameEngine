#pragma once

#include "Render/Buffer/gl_alias.hpp"
#include "Core/transparent_hash_table.hpp"

#include <GL/glew.h>
class Uniform{
    Uniform(ShaderGPUID shader_id, const char* uniform_name);
    
    template<typename T> void setValue(const T& value) const;
private:
    GLuint ID = -1;
};

inline Uniform::Uniform(ShaderGPUID shader_id, const char* uniform_name):
    ID(glGetUniformLocation(shader_id, uniform_name)){}