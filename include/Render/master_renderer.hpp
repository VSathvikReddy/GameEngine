#pragma once

#include "Render/Sprite/sprite_renderer.hpp"
#include "Shader/shader.hpp"

#include "Render/camera.hpp"

class MasterRenderer{
public:
    static void RegisterMaster(MasterRenderer* instance) noexcept{ master = instance;}

    template<typename T>
    static void SubmitRender(const T& object){}
    static void SubmitRender(const Sprite& object){ master->sprite_renderer.acceptRenderOrder(object);}

    static void Render(const Camera& cam){
        master->sprite_renderer.render(master->shader);
    }

    static void PreparePipeline(const Camera& camera, int windowWidth, int windowHeight) noexcept;

    static Renderer<Sprite>& getSpriteRenderer(){
        return master->sprite_renderer;
    }
private:
    Renderer<Sprite> sprite_renderer; 
    Shader shader;

    uint32_t m_uboGlobalMatrices = 0;

    inline static MasterRenderer* master = nullptr;
};


template<typename T>
class Renderer;