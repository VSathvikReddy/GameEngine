#pragma once

#include <cstdint>
#include <compare>
#include <type_traits>

template<typename T>
class Time {
    static_assert(std::is_floating_point_v<T>, "Time representation must use a floating-point type.");
private:
    T m_milliseconds = T{0};

    // PRIVATE CONSTRUCTOR
    constexpr explicit Time(T milliseconds) noexcept : m_milliseconds(milliseconds) {}

public:
    constexpr Time() noexcept = default;
    ~Time() = default;


    [[nodiscard]] static constexpr Time Microseconds(T us) noexcept { return Time(us / T{1000}); }
    [[nodiscard]] static constexpr Time Milliseconds(T ms) noexcept { return Time(ms); }
    [[nodiscard]] static constexpr Time Seconds(T s)       noexcept { return Time(s * T{1000}); }
    [[nodiscard]] static constexpr Time Minutes(T mins)    noexcept { return Time(mins * T{60000}); }
    [[nodiscard]] static constexpr Time Hours(T hrs)       noexcept { return Time(hrs * T{3600000}); }


    [[nodiscard]] constexpr T asMicroseconds() const noexcept { return m_milliseconds * T{1000}; }
    [[nodiscard]] constexpr T asMilliseconds() const noexcept { return m_milliseconds; }
    [[nodiscard]] constexpr T asSeconds()      const noexcept { return m_milliseconds / T{1000}; }
    [[nodiscard]] constexpr T asMinutes()      const noexcept { return m_milliseconds / T{60000}; }
    [[nodiscard]] constexpr T asHours()        const noexcept { return m_milliseconds / T{3600000}; }


    constexpr Time operator+(const Time& rhs) const noexcept { return Time(m_milliseconds + rhs.m_milliseconds); }
    constexpr Time operator-(const Time& rhs) const noexcept { return Time(m_milliseconds - rhs.m_milliseconds); }
    constexpr Time operator-()                const noexcept { return Time(-m_milliseconds); }

    constexpr Time& operator+=(const Time& rhs) noexcept { m_milliseconds += rhs.m_milliseconds; return *this; }
    constexpr Time& operator-=(const Time& rhs) noexcept { m_milliseconds -= rhs.m_milliseconds; return *this; }

    constexpr Time operator*(T scalar) const noexcept { return Time(m_milliseconds * scalar); }
    constexpr Time operator/(T scalar) const noexcept { return Time(m_milliseconds / scalar); }
    constexpr Time& operator*=(T scalar) noexcept { m_milliseconds *= scalar; return *this; }
    constexpr Time& operator/=(T scalar) noexcept { m_milliseconds /= scalar; return *this; }

    
    constexpr bool operator==(const Time& rhs) const noexcept { return m_milliseconds == rhs.m_milliseconds; }
    constexpr bool operator!=(const Time& rhs) const noexcept { return m_milliseconds != rhs.m_milliseconds; }
    constexpr bool operator< (const Time& rhs) const noexcept { return m_milliseconds <  rhs.m_milliseconds; }
    constexpr bool operator<=(const Time& rhs) const noexcept { return m_milliseconds <= rhs.m_milliseconds; }
    constexpr bool operator> (const Time& rhs) const noexcept { return m_milliseconds >  rhs.m_milliseconds; }
    constexpr bool operator>=(const Time& rhs) const noexcept { return m_milliseconds >= rhs.m_milliseconds; }
};

using TimeF = Time<float>;
using TimeD = Time<double>;

// Modern User-Defined Literals for ultra-clean timelines
constexpr TimeF operator""_ms(long double ms) { return TimeF::Milliseconds(static_cast<float>(ms)); }
constexpr TimeF operator""_s(long double s)   { return TimeF::Seconds(static_cast<float>(s)); }