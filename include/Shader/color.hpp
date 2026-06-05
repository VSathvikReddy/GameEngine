#pragma once

#include <cstdint>
#include <iostream>
#include <algorithm>

class Color {
public:
    // Memory layout matches graphics card expectations (RGBA)
    union {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
        uint32_t packedValue; // All 4 channels packed into a single 32-bit integer
    };

    // ====================================================================
    // 1. Constructors (SFML-Inspired style)
    // ====================================================================
    
    constexpr Color() noexcept : r(255), g(255), b(255), a(255) {}
    constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) noexcept: r(red), g(green), b(blue), a(alpha) {}
    constexpr explicit Color(uint32_t packedRGBA) noexcept: packedValue(packedRGBA){}

    ~Color() = default;

    constexpr Color(const Color&) noexcept = default;
    constexpr Color& operator=(const Color&) noexcept = default;
    constexpr Color(Color&&) noexcept = default;
    constexpr Color& operator=(Color&&) noexcept = default;

    
    // SFML component-wise color addition (saturates up to 255)
    Color operator+(const Color& other) const noexcept;
    Color& operator+=(const Color& other) noexcept;

    // SFML component-wise color subtraction (saturates down to 0)
    Color operator-(const Color& other) const noexcept;
    Color& operator-=(const Color& other) noexcept;

    // SFML component-wise modulation (multiplies normalized ratios)
    Color operator*(const Color& other) const noexcept;
    Color& operator*=(const Color& other) noexcept;

    // Scalar scaling
    Color operator*(float factor) const noexcept;
    Color& operator*=(float factor) noexcept;

    constexpr bool operator==(const Color& other) const noexcept { return packedValue == other.packedValue; }
    constexpr bool operator!=(const Color& other) const noexcept { return packedValue != other.packedValue; }


    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Magenta;
    static const Color Cyan;
    static const Color Transparent;
};

static_assert(sizeof(Color) == 4, "CRITICAL: Color class footprint must be exactly 4 bytes!");