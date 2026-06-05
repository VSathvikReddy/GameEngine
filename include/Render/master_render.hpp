#include "Render/Shader/shader_manager.hpp"
#include "Render/camera.hpp"
class Camera;

//Master Renderer
class Render{
private:
    Render(){
        glGenBuffers(1, &cam_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, cam_ubo);
        glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); // allocate 66 + 64, view + projection
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, shader_manager.setUBOs("Camera"), cam_ubo, 0, 2 * sizeof(glm::mat4));
    }
    ~Render(){
        if (cam_ubo == 0) return;

        glDeleteBuffers(1, &cam_ubo);
        cam_ubo = 0;
    }

    static void RegisterMaster(Render* instance){ master = instance; }

    static void SetCamera(const Camera& cam){
        glBindBuffer(GL_UNIFORM_BUFFER, master->cam_ubo);

        std::array<glm::mat4,2> cam_data = {cam.getViewMatrix(),cam.getProjectionMatrix()};
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 2*sizeof(glm::mat4), cam_data.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);  
    }
    template<typename T> static void Submit(const T& renderable);
    static void Flush(){

    }

    void LoadShader(const std::filesystem::path& folder, std::string_view shaderName, std::string_view userDefines = ""){
        shader_manager.loadShader(folder,shaderName,userDefines);
    }
public:
    ShaderManager shader_manager;

    uint32_t cam_ubo = 0;








    inline static Render* master = nullptr;
};