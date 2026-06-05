#include "Render/color.hpp"

// ====================================================================
// SFML Static Constant Definitions
// ====================================================================
const Color Color::Black{0, 0, 0, 255};
const Color Color::White{255, 255, 255, 255};
const Color Color::Red{255, 0, 0, 255};
const Color Color::Green{0, 255, 0, 255};
const Color Color::Blue{0, 0, 255, 255};
const Color Color::Yellow{255, 255, 0, 255};
const Color Color::Magenta{255, 0, 255, 255};
const Color Color::Cyan{0, 255, 255, 255};
const Color Color::Transparent{0, 0, 0, 0};

// ====================================================================
// Color Addition Engine (Saturates safely at 255 boundary limits)
// ====================================================================
Color Color::operator+(const Color& other) const noexcept {
    return Color(
        static_cast<uint8_t>(std::min(static_cast<int>(r) + other.r, 255)),
        static_cast<uint8_t>(std::min(static_cast<int>(g) + other.g, 255)),
        static_cast<uint8_t>(std::min(static_cast<int>(b) + other.b, 255)),
        static_cast<uint8_t>(std::min(static_cast<int>(a) + other.a, 255))
    );
}

Color& Color::operator+=(const Color& other) noexcept {
    *this = *this + other;
    return *this;
}

// ====================================================================
// Color Subtraction Engine (Clamps safely at 0 boundary floor)
// ====================================================================
Color Color::operator-(const Color& other) const noexcept {
    return Color(
        static_cast<uint8_t>(std::max(static_cast<int>(r) - other.r, 0)),
        static_cast<uint8_t>(std::max(static_cast<int>(g) - other.g, 0)),
        static_cast<uint8_t>(std::max(static_cast<int>(b) - other.b, 0)),
        static_cast<uint8_t>(std::max(static_cast<int>(a) - other.a, 0))
    );
}

Color& Color::operator-=(const Color& other) noexcept {
    *this = *this - other;
    return *this;
}

// ====================================================================
// Color Modulation Engine (Component multiplication normalized ratios)
// ====================================================================
Color Color::operator*(const Color& other) const noexcept {
    return Color(
        static_cast<uint8_t>((static_cast<int>(r) * other.r) / 255),
        static_cast<uint8_t>((static_cast<int>(g) * other.g) / 255),
        static_cast<uint8_t>((static_cast<int>(b) * other.b) / 255),
        static_cast<uint8_t>((static_cast<int>(a) * other.a) / 255)
    );
}

Color& Color::operator*=(const Color& other) noexcept {
    *this = *this * other;
    return *this;
}

// ====================================================================
// Scalar Scale Engine
// ====================================================================
Color Color::operator*(float factor) const noexcept {
    return Color(
        static_cast<uint8_t>(std::clamp(static_cast<float>(r) * factor, 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(static_cast<float>(g) * factor, 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(static_cast<float>(b) * factor, 0.0f, 255.0f)),
        a // Retain original transparency alpha structure unchanged by scalars
    );
}

Color& Color::operator*=(float factor) noexcept {
    *this = *this * factor;
    return *this;
}