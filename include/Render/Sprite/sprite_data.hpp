#pragma once

#include "Render/Sprite/sprite_sheet.hpp"
#include "Render/color.hpp"

#include "Geometry/vector2.hpp"
#include "Geometry/angle.hpp"

struct SpriteData {
    Vec2f    position   = {0.0f, 0.0f};
    Vec2f    dimensions = {1.0f, 1.0f};
    AngleF   rotation;
    Color    color      = Color::White;
    uint16_t sheetID    = 0;
    uint16_t regionID   = 0;
};


#pragma pack(push, 1)
struct SpriteGPUData {
    Vec2f position;
    Vec2f right;
    Vec2f up;
    SpriteRegionID regionID;
    Color color;
};
#pragma pack(pop)
static_assert(sizeof(SpriteGPUData) == 32, "CRITICAL: SpriteGPUData broke the 32-byte layout boundary limit!");