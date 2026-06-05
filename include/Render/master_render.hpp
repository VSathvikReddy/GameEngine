#include "Render/Shader/shader_manager.hpp"

//Master Renderer
class Render{
private:
    Render() = default;
    ~Render() = default;

    static void RegisterMaster(Render* instance){ master = instance; }
    static void LoadShader(const std::filesystem::path& folder, std::string_view shaderName, std::string_view userDefines = ""){
        master->shader_manager.loadShader(folder,shaderName,userDefines);
    }
public:
    ShaderManager shader_manager;
    








    inline static Render* master = nullptr;
};