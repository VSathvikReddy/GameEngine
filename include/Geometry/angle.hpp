#pragma once

#include <cmath>
#include <compare>
#include <type_traits>

template<typename T = float>
class Angle {
    static_assert(std::is_floating_point_v<T>, "Angle representation must use a floating-point type.");

private:
    T m_radians = T{0};

    // PRIVATE CONSTRUCTOR
    constexpr explicit Angle(T radians) noexcept : m_radians(radians) {}

public:
    static constexpr T PI         = static_cast<T>(3.14159265358979323846);
    static constexpr T TWO_PI     = PI * static_cast<T>(2);
    static constexpr T DEG_TO_RAD = PI / static_cast<T>(180);
    static constexpr T RAD_TO_DEG = static_cast<T>(180) / PI;

    constexpr Angle() noexcept = default;
    ~Angle() = default;

    template<typename U>
    [[nodiscard]] static constexpr Angle Radians(U rad) noexcept { 
        return Angle(static_cast<T>(rad)); 
    }
    
    template<typename U>
    [[nodiscard]] static constexpr Angle Degrees(U deg) noexcept { 
        return Angle(static_cast<T>(deg) * DEG_TO_RAD); 
    }

    // =====================================================================
    // EXPLICIT GETTERS
    // =====================================================================
    [[nodiscard]] constexpr T asRadians() const noexcept { return m_radians; }
    [[nodiscard]] constexpr T asDegrees() const noexcept { return m_radians * RAD_TO_DEG; }

    // =====================================================================
    // NORMALIZATION
    // =====================================================================
    [[nodiscard]] Angle wrapped2PI() const noexcept {
        T r = std::fmod(m_radians, TWO_PI);
        if (r < T{0}) r += TWO_PI;
        return Angle(r);
    }

    [[nodiscard]] Angle wrappedPI() const noexcept {
        T r = std::fmod(m_radians + PI, TWO_PI);
        if (r < T{0}) r += TWO_PI;
        return Angle(r - PI);
    }

    // Direct performance math targets
    [[nodiscard]] T sin() const noexcept { return std::sin(m_radians); }
    [[nodiscard]] T cos() const noexcept { return std::cos(m_radians); }
    [[nodiscard]] T tan() const noexcept { return std::tan(m_radians); }
    
    [[nodiscard]] static T sin(const Angle<T>& a) noexcept { return a.sin(); }
    [[nodiscard]] static T cos(const Angle<T>& a) noexcept { return a.cos(); }
    [[nodiscard]] static T tan(const Angle<T>& a) noexcept { return a.tan(); }

    [[nodiscard]] static Angle atan2(T y, T x) noexcept { return Angle(std::atan2(y, x)); }

    constexpr Angle operator+(const Angle& rhs) const noexcept { return Angle(m_radians + rhs.m_radians); }
    constexpr Angle operator-(const Angle& rhs) const noexcept { return Angle(m_radians - rhs.m_radians); }
    constexpr Angle operator-()                const noexcept { return Angle(-m_radians); }

    constexpr Angle& operator+=(const Angle& rhs) noexcept { m_radians += rhs.m_radians; return *this; }
    constexpr Angle& operator-=(const Angle& rhs) noexcept { m_radians -= rhs.m_radians; return *this; }

    constexpr Angle operator*(T scalar) const noexcept { return Angle(m_radians * scalar); }
    constexpr Angle operator/(T scalar) const noexcept { return Angle(m_radians / scalar); }
    constexpr Angle& operator*=(T scalar) noexcept { m_radians *= scalar; return *this; }
    constexpr Angle& operator/=(T scalar) noexcept { m_radians /= scalar; return *this; }

    constexpr bool operator==(const Angle& rhs) const noexcept { return m_radians == rhs.m_radians; }
    constexpr bool operator!=(const Angle& rhs) const noexcept { return m_radians != rhs.m_radians; }
    constexpr bool operator< (const Angle& rhs) const noexcept { return m_radians <  rhs.m_radians; }
    constexpr bool operator<=(const Angle& rhs) const noexcept { return m_radians <= rhs.m_radians; }
    constexpr bool operator> (const Angle& rhs) const noexcept { return m_radians >  rhs.m_radians; }
    constexpr bool operator>=(const Angle& rhs) const noexcept { return m_radians >= rhs.m_radians; }
};

using AngleF = Angle<float>;
using AngleD = Angle<double>;


// Floating-point Literals (e.g., 90.0_deg, 1.57_rad)
constexpr AngleF operator""_deg(long double deg) { return AngleF::Degrees(static_cast<float>(deg)); }
constexpr AngleF operator""_rad(long double rad) { return AngleF::Radians(static_cast<float>(rad)); }

// Integer Literals (e.g., 90_deg, 3_rad)
constexpr AngleF operator""_deg(unsigned long long int deg) { return AngleF::Degrees(static_cast<float>(deg)); }
constexpr AngleF operator""_rad(unsigned long long int rad) { return AngleF::Radians(static_cast<float>(rad)); }