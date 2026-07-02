#include "Render/Buffer/shader_manager.hpp"

#include <GL/glew.h>


#include <fstream>
#include <iostream>

namespace {
    std::string ReadFile(const std::filesystem::path& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open shader file: " << filepath << '\n';
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // Helper 2: Compile a single shader and check for errors
    GLuint CompileShader(GLenum type, const std::string& source, const std::string& name) {
        if (source.empty()) return 0;

        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        // Check compilation status
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, nullptr, log.data());
            std::cerr << "Shader Compilation Failed (" << name << "):\n" << log << '\n';
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }
}