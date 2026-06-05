#pragma once

#include "Render/Sprite/sprite_sheet.hpp"
#include "Render/Sprite/sprite_data.hpp"

#include "Geometry/vector2.hpp"
#include "Render/color.hpp"

#include "Core/free_list.hpp"

#include <cstdint>

class Shader;
class Sprite;
struct SpriteSheet;

class Camera;

template<typename T> class Renderer;

template<>
class Renderer<Sprite>{
public:

    Renderer(const Shader& compatibleShader);
    ~Renderer();

    void acceptRenderOrder(const Sprite& object);


    void loadSpriteSheet(const std::string& folderPath, const std::string& assetName);

    void render(Shader& shader);

private:
    friend class Sprite;
    uint32_t m_quad_vao = 0;
    uint32_t m_quad_vbo = 0;
    uint32_t m_id_vbo   = 0;
    uint32_t m_uboPaletteBlock = 0;
    static constexpr size_t MAX_BATCH_CAPACITY = 2000;

    
    std::vector<SpriteSheet> m_sheets;
    std::unordered_map<std::string, uint16_t> m_sheetPathRegistry;

    std::unordered_map<unsigned int, std::vector<SpriteGPUData>> m_buckets;

    FreeList<SpriteData> actual_data;
};