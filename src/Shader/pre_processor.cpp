#include "Shader/pre_processor.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
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


namespace ShaderPreProcessor {
    std::string Process(const std::string& filePath, const std::string& userDefines) {
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

}