#pragma once

#include "Render/color.hpp"
#include "Render/Sprite/sprite_sheet.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include "Geometry/vector2.hpp"
#include "Geometry/angle.hpp"

class Sprite;

template<typename T> class Renderer;
template<> class Renderer<Sprite>;

class Sprite {
public:
    Sprite() noexcept = delete;    
    static Sprite Create(std::string_view sheet);
    static Sprite Create(SpriteSheetID id);
    ~Sprite();

    // Trivial copy and move operations (safe and instant because it's just an int)
    Sprite(const Sprite&) noexcept;
    Sprite& operator=(const Sprite&);
    Sprite(Sprite&&) noexcept;
    Sprite& operator=(Sprite&&) noexcept;

    [[nodiscard]] constexpr bool isValid() const noexcept { return id != 0; }
    
    void setPosition(Vec2f position) noexcept;
    void move(Vec2f offset) noexcept;
    [[nodiscard]] Vec2f getPosition() const noexcept;

    void setScale(Vec2f scale) noexcept;
    [[nodiscard]] Vec2f getScale() const noexcept;
    void setWidth(float width) noexcept;
    [[nodiscard]]  float getWidth() const noexcept;
    void setHeight(float height) noexcept;
    [[nodiscard]] float getHeight() const noexcept;

    void setRotation(AngleF radians) noexcept;
    void rotate(AngleF deltaRadians) noexcept;
    [[nodiscard]] AngleF getRotation() const noexcept;

    void setColor(Color other) noexcept;
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;
    void setAlpha(uint8_t alpha) noexcept;
    [[nodiscard]] Color getColor() const noexcept;

    void setFrame(std::string_view frameKey) noexcept;
    void setFrame(SpriteRegionID ID) noexcept;
    
    [[nodiscard]] SpriteSheetID getSheetID() const noexcept;
    [[nodiscard]] SpriteRegionID getRegionID() const noexcept;

private:
    friend class Renderer<Sprite>;
    constexpr explicit Sprite(uint32_t handleID) noexcept : id(handleID) {}
    uint32_t id = 0;
};

