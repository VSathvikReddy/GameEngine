#include "Render/Sprite/sprite.hpp"
#include "Render/Sprite/sprite_renderer.hpp"
#include "Render/master_renderer.hpp"

Sprite Sprite::Create(std::string_view sheet) {
    auto& renderer = MasterRenderer::getSpriteRenderer();
    auto it = renderer.m_sheetPathRegistry.find(std::string(sheet));
    if (it != renderer.m_sheetPathRegistry.end()) {
        return Sprite::Create(it->second);
    }
    return Sprite(0);
}

Sprite Sprite::Create(SpriteSheetID id) {
    SpriteData initialData;
    initialData.sheetID = id;
    
    auto& renderer = MasterRenderer::getSpriteRenderer();
    uint32_t assignedHandle = renderer.actual_data.insert(initialData);
    return Sprite(assignedHandle);
}

Sprite::~Sprite() {
    if (isValid()) {
        auto& renderer = MasterRenderer::getSpriteRenderer();
        renderer.actual_data.erase(id);
        id = 0;
    }
}

Sprite::Sprite(const Sprite& other) noexcept {
    if (other.isValid()) {
        auto& renderer = MasterRenderer::getSpriteRenderer();
        SpriteData copyData = renderer.actual_data.at(other.id);
        this->id = renderer.actual_data.insert(copyData);
    } else {
        this->id = 0;
    }
}

Sprite& Sprite::operator=(const Sprite& other) {
    if (this == &other) return *this;
    
    auto& renderer = MasterRenderer::getSpriteRenderer();
    if (isValid()) {
        renderer.actual_data.erase(id);
    }
    
    if (other.isValid()) {
        SpriteData copyData = renderer.actual_data.at(other.id);
        this->id = renderer.actual_data.insert(copyData);
    } else {
        this->id = 0;
    }
    return *this;
}

Sprite::Sprite(Sprite&& other) noexcept : id(other.id) {
    other.id = 0;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
    if (this != &other) {
        auto& renderer = MasterRenderer::getSpriteRenderer();
        if (isValid()) {
            renderer.actual_data.erase(id);
        }
        id = other.id;
        other.id = 0;
    }
    return *this;
}

void Sprite::setPosition(Vec2f position) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).position = position;
}

void Sprite::move(Vec2f offset) noexcept {
    auto& data = MasterRenderer::getSpriteRenderer().actual_data.at(id);
    data.position.x += offset.x;
    data.position.y += offset.y;
}

Vec2f Sprite::getPosition() const noexcept {
    return MasterRenderer::getSpriteRenderer().actual_data.at(id).position;
}

void Sprite::setScale(Vec2f scale) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).dimensions = scale;
}

Vec2f Sprite::getScale() const noexcept {
    return MasterRenderer::getSpriteRenderer().actual_data.at(id).dimensions;
}

void Sprite::setWidth(float width) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).dimensions.x = width;
}

float Sprite::getWidth() const noexcept {
    return MasterRenderer::getSpriteRenderer().actual_data.at(id).dimensions.x;
}

void Sprite::setHeight(float height) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).dimensions.y = height;
}

float Sprite::getHeight() const noexcept {
    return MasterRenderer::getSpriteRenderer().actual_data.at(id).dimensions.y;
}

void Sprite::setRotation(AngleF radians) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).rotation = radians;
}

void Sprite::rotate(AngleF deltaRadians) noexcept {
    auto& data = MasterRenderer::getSpriteRenderer().actual_data.at(id);
    data.rotation += deltaRadians;
}

AngleF Sprite::getRotation() const noexcept {
    return MasterRenderer::getSpriteRenderer().actual_data.at(id).rotation;
}

void Sprite::setColor(Color other) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).color = other;
}

void Sprite::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).color = Color{r, g, b, a};
}

void Sprite::setAlpha(uint8_t alpha) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).color.a = alpha;
}

Color Sprite::getColor() const noexcept {
    return MasterRenderer::getSpriteRenderer().actual_data.at(id).color;
}

void Sprite::setFrame(std::string_view frameKey) noexcept {
    auto& renderer = MasterRenderer::getSpriteRenderer();
    auto& data = renderer.actual_data.at(id);
    const auto& sheet = renderer.m_sheets[data.sheetID];
    auto it = sheet.stringToRegionIdMap.find(std::string(frameKey));
    if (it != sheet.stringToRegionIdMap.end()) {
        data.regionID = static_cast<uint16_t>(it->second);
    }
}

void Sprite::setFrame(SpriteRegionID ID) noexcept {
    MasterRenderer::getSpriteRenderer().actual_data.at(id).regionID = static_cast<uint16_t>(ID);
}

SpriteSheetID Sprite::getSheetID() const noexcept {
    return MasterRenderer::getSpriteRenderer().actual_data.at(id).sheetID;
}

SpriteRegionID Sprite::getRegionID() const noexcept {
    return static_cast<SpriteRegionID>(MasterRenderer::getSpriteRenderer().actual_data.at(id).regionID);
}