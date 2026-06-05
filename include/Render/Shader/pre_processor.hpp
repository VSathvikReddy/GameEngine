#pragma once

#include <string>

namespace ShaderPreProcessor{
    std::string Process(const std::string& filePath, const std::string& userDefines = "");
}