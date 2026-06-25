#include "Render/Buffer/shader_manager.hpp"

#include <iostream>

inline void assertShaderFilesExist([[maybe_unused]] const std::filesystem::path& vertPath,   [[maybe_unused]] const std::filesystem::path& fragPath) noexcept {
#ifndef NDEBUG
    if (!std::filesystem::exists(vertPath)) {
        std::cerr << "[Shader Error] Missing Vertex Shader File at: " << vertPath << "\n";
        exit(1);
    }
    if (!std::filesystem::exists(fragPath)) {
        std::cerr << "[Shader Error] Missing Fragment Shader File at: " << fragPath << "\n";
        exit(1);
    }
#endif
}

// 2. Preprocessor output validation
inline void assertShaderPreProcessSucess([[maybe_unused]] const std::string& vertSource, [[maybe_unused]] const std::string& fragSource, [[maybe_unused]] const std::string& shaderName) noexcept {
#ifndef NDEBUG
    if (vertSource.empty() || fragSource.empty()) {                                
        std::cerr << "ERROR::SHADER::MANAGER::PREPROCESS_FAILED for: " << shaderName << "\n";                                                                                    
        exit(1);                                                                      
    }    
#endif
}

// 3. Driver compilation validation
inline void assertShaderCompilationSuccess([[maybe_unused]] unsigned int vertID, [[maybe_unused]] unsigned int fragID) noexcept {
#ifndef NDEBUG
    if (vertID == 0 || fragID == 0) {
        if (vertID != 0) glDeleteShader(vertID);
        if (fragID != 0) glDeleteShader(fragID);
        assert(false && "CRITICAL: Compilation failed for one or more shader stages! Inspect console outputs.");
    }
#endif
}


// 4. Validate individual shader compilation stage logs
inline void assertShaderStageCompiled([[maybe_unused]] unsigned int shaderID,  [[maybe_unused]] GLenum type) noexcept {
#ifndef NDEBUG
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        const char* shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : 
                                 (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "UNKNOWN";
        std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << "\n";
        exit(1);
    }
#endif
}

// 5. Validate total pipeline program linking logs
inline void assertProgramLinked([[maybe_unused]] unsigned int programID) noexcept {
#ifndef NDEBUG
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
        exit(1);
    }
#endif
}







std::string PreProcess(const std::string& filePath, const std::string& userDefines = "");
unsigned int createShader(GLenum type, const char* shaderSource, char* infoLog);
unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader, char* infoLog);






ShaderID ShaderManager::loadShader(const std::filesystem::path& folder, const std::string& shaderName, std::string_view userDefines) noexcept {    
    assert(string_to_shader_ID.find(shaderName) == string_to_shader_ID.end() && "Same shader loaded twice or different shader with same name");

    std::filesystem::path vertFilePath = folder / (shaderName + ".vert");
    std::filesystem::path fragFilePath = folder / (shaderName + ".frag");

    assertShaderFilesExist(vertFilePath, fragFilePath);

    std::string vertexShaderSource = PreProcess(vertFilePath.string(), std::string(userDefines));
    std::string fragmentShaderSource = PreProcess(fragFilePath.string(), std::string(userDefines));

    assertShaderPreProcessSucess(vertexShaderSource, fragmentShaderSource, shaderName); if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {return ERROR_SHADER;}

#ifndef NDEBUG
    char infoLog[512];
    char* infoLogPtr = infoLog;
#else
    char* infoLogPtr = nullptr;
#endif

    unsigned int vertexShaderID = createShader(GL_VERTEX_SHADER, vertexShaderSource.c_str(), infoLogPtr);
    unsigned int fragmentShaderID = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str(), infoLogPtr);

    assertShaderCompilationSuccess(vertexShaderID, fragmentShaderID);
    if (vertexShaderID == 0 || fragmentShaderID == 0) {
        if (vertexShaderID != 0) glDeleteShader(vertexShaderID);
        if (fragmentShaderID != 0) glDeleteShader(fragmentShaderID);
        return ERROR_SHADER; 
    }

    ShaderGPUID ID = linkShaders(vertexShaderID, fragmentShaderID, infoLogPtr);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    assert(ID != 0 && "Unable to link the Shaders");
    if (ID == 0) return ERROR_SHADER;

    ShaderID newEngineID = static_cast<ShaderID>(shader_data.size());
    shader_data.push_back({ID});
    string_to_shader_ID[shaderName] = newEngineID;

    for(const auto& itr: registered_ubos){
        unsigned int idx = glGetUniformBlockIndex(ID, itr.first.c_str());
        if (idx != GL_INVALID_INDEX) {
            glUniformBlockBinding(ID, idx, itr.second.globalSlot);
        }
    }
    return newEngineID;
}

unsigned int createShader(GLenum type, const char* shaderSource, [[maybe_unused]] char* infoLogPtr) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    assertShaderStageCompiled(shader, type);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glDeleteShader(shader); 
        return 0;               
    }

    return shader;
}

ShaderGPUID linkShaders(unsigned int vertexShader, unsigned int fragmentShader, [[maybe_unused]] char* infoLogPtr) {
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    assertProgramLinked(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteProgram(shaderProgram); 
        return 0;
    }
    
    return shaderProgram;
}















#include <regex>
#include <fstream>
#include <unordered_set>



std::string ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()){
        std::cerr<<"Shader file not found:"<<path<<'\n';
        return "";
    } 
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string ExtractDirectory(const std::string& path) {
    size_t found = path.find_last_of("/\\");
    return (found != std::string::npos) ? path.substr(0, found + 1) : "";
}

void ResolveIncludes(std::string& source, const std::string& directory, std::unordered_set<std::string>& includedFiles) {
    std::regex includeRegex(R"(^\s*#include\s+["<]([^">]+)[">])", std::regex::multiline);
    std::smatch match;

    while (std::regex_search(source, match, includeRegex)) {
        std::string includePath = directory + match[1].str();
        if (includedFiles.find(includePath) != includedFiles.end()) {
            source.replace(match.position(), match.length(), "");
            continue;
        }
        includedFiles.insert(includePath);
        std::string includeContent = ReadFile(includePath);
        
        // Strip nested versions
        std::regex versionRegex(R"(^\s*#version\s+\d+.*)", std::regex::multiline);
        includeContent = std::regex_replace(includeContent, versionRegex, "");

        ResolveIncludes(includeContent, ExtractDirectory(includePath), includedFiles);
        source.replace(match.position(), match.length(), includeContent);
    }
}


std::string PreProcess(const std::string& filePath, const std::string& userDefines) {
    std::string source = ReadFile(filePath);
    std::regex versionRegex(R"(^\s*#version\s+\d+\s*\w*)", std::regex::multiline);
    std::smatch match;

    std::string finalSource;
    if (std::regex_search(source, match, versionRegex)) {
        std::string versionLine = match.str();
        std::string remainingSource = source.substr(match.position() + match.length());
        finalSource = versionLine + "\n" + userDefines + "\n" + remainingSource;
    } else {
        finalSource = userDefines + "\n" + source;
    }

    std::unordered_set<std::string> includedFiles;
    ResolveIncludes(finalSource, ExtractDirectory(filePath), includedFiles);
    return finalSource;
}