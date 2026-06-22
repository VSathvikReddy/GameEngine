#pragma once

#include "Geometry/vector2.hpp"

#include <algorithm>
#include <cassert>

template <typename T>
class Rect {
public:
    constexpr Rect();
    constexpr Rect(T min_x, T min_y, T width, T height);
    constexpr Rect(const Vec2<T>& position, const Vec2<T>& size);

    template <typename U>
    explicit constexpr Rect(const Rect<U>& other);

    [[nodiscard]] static constexpr Rect<T> fromMinMax(T min_x, T min_y, T max_x, T max_y);
    [[nodiscard]] static constexpr Rect<T> fromMinMax(const Vec2<T>& min_p, const Vec2<T>& max_p);


    template<bool IsMinX = true, bool IsMinY = true>
    [[nodiscard]] constexpr Vec2<T> getPosition() const;
    [[nodiscard]] constexpr Vec2<T> getCenter() const;
    [[nodiscard]] constexpr Vec2<T> getSize() const;


#pragma region Setters
    template <bool IsMinX = true, bool IsMinY = true> constexpr Rect<T>& setPosition(T x, T y) &;
    template <bool IsMinX = true, bool IsMinY = true> constexpr Rect<T>  setPosition(T x, T y) &&;
    template <bool IsMinX = true, bool IsMinY = true> constexpr Rect<T>& setPosition(const Vec2<T>& pos) &;
    template <bool IsMinX = true, bool IsMinY = true> constexpr Rect<T>  setPosition(const Vec2<T>& pos) &&;

    constexpr Rect<T>& setCenter(T x, T y) &;
    constexpr Rect<T>  setCenter(T x, T y) &&;
    constexpr Rect<T>& setCenter(const Vec2<T>& center) &;
    constexpr Rect<T>  setCenter(const Vec2<T>& center) &&;

    constexpr Rect<T>& setSize(T w, T h) &;
    constexpr Rect<T>  setSize(T w, T h) &&;
    constexpr Rect<T>& setSize(const Vec2<T>& size) &;
    constexpr Rect<T>  setSize(const Vec2<T>& size) &&;

    constexpr Rect<T>& scaleSize(T s) &;
    constexpr Rect<T>  scaleSize(T s) &&;
    constexpr Rect<T>& scaleSize(const Vec2<T>& s) &;
    constexpr Rect<T>  scaleSize(const Vec2<T>& s) &&;
#pragma endregion Setters


    [[nodiscard]] constexpr bool contains(T x, T y) const;
    [[nodiscard]] constexpr bool contains(const Vec2<T>& point) const;

    [[nodiscard]] constexpr bool intersects(const Rect<T>& other) const;
    constexpr bool intersects(const Rect<T>& other, Rect<T>& out_intersection) const;

private:
    T m_min_x;   
    T m_min_y;    
    T m_width;  
    T m_height; 
};


// Global Operators
template <typename T>
[[nodiscard]] constexpr bool operator==(const Rect<T>& a, const Rect<T>& b);

template <typename T>
[[nodiscard]] constexpr bool operator!=(const Rect<T>& a, const Rect<T>& b);






template <typename T>
constexpr Rect<T>::Rect() : m_min_x(0), m_min_y(0), m_width(0), m_height(0) {}

template <typename T>
constexpr Rect<T>::Rect(T min_x, T min_y, T width, T height)
    : m_min_x(min_x), m_min_y(min_y), m_width(width), m_height(height) {}

template <typename T>
constexpr Rect<T>::Rect(const Vec2<T>& position, const Vec2<T>& size)
    : m_min_x(position.data[0]), m_min_y(position.data[1]), 
      m_width(size.data[0]), m_height(size.data[1]) {}

template <typename T>
template <typename U>
constexpr Rect<T>::Rect(const Rect<U>& other)
    : m_min_x(static_cast<T>(other.getX())), m_min_y(static_cast<T>(other.getY())),
      m_width(static_cast<T>(other.getWidth())), m_height(static_cast<T>(other.getHeight())) {}

template <typename T>
constexpr Rect<T> Rect<T>::fromMinMax(T min_x, T min_y, T max_x, T max_y) {
    return Rect<T>(min_x, min_y, max_x - min_x, max_y - min_y);
}

template <typename T>
constexpr Rect<T> Rect<T>::fromMinMax(const Vec2<T>& min_p, const Vec2<T>& max_p) {
    return Rect<T>(min_p, max_p - min_p);
}

template <typename T>
template<bool IsMinX, bool IsMinY>
constexpr Vec2<T> Rect<T>::getPosition() const {
    if constexpr (IsMinX && IsMinY) return Vec2<T>(m_min_x, m_min_y);
    else if constexpr (!IsMinX && IsMinY) return Vec2<T>(m_min_x + m_width, m_min_y);
    else if constexpr (IsMinX && !IsMinY) return Vec2<T>(m_min_x, m_min_y + m_height);
    else return Vec2<T>(m_min_x + m_width, m_min_y + m_height);
}

template <typename T>
constexpr Vec2<T> Rect<T>::getCenter() const {
    return Vec2<T>(m_min_x + (m_width / T(2)), m_min_y + (m_height / T(2)));
}

template <typename T>
constexpr Vec2<T> Rect<T>::getSize() const { 
    return Vec2<T>(m_width, m_height); 
}








template <typename T>
template <bool IsMinX, bool IsMinY>
constexpr Rect<T>& Rect<T>::setPosition(T x, T y) & {
    if constexpr (IsMinX) {
        m_min_x = x;
    } else {
        m_min_x = x - m_width;
    }

    if constexpr (IsMinY) {
        m_min_y = y;
    } else {
        m_min_y = y - m_height;
    }
    return *this;
}

template <typename T>
template <bool IsMinX, bool IsMinY>
constexpr Rect<T> Rect<T>::setPosition(T x, T y) && {
    return std::move(this->setPosition<IsMinX, IsMinY>(x, y));
}

template <typename T>
template <bool IsMinX, bool IsMinY>
constexpr Rect<T>& Rect<T>::setPosition(const Vec2<T>& pos) & {
    return setPosition<IsMinX, IsMinY>(pos.data[0], pos.data[1]);
}

template <typename T>
template <bool IsMinX, bool IsMinY>
constexpr Rect<T> Rect<T>::setPosition(const Vec2<T>& pos) && {
    return std::move(this->setPosition<IsMinX, IsMinY>(pos.data[0], pos.data[1]));
}

// --- setCenter ---
template <typename T>
constexpr Rect<T>& Rect<T>::setCenter(T x, T y) & {
    m_min_x = x - (m_width / T(2));
    m_min_y = y - (m_height / T(2));
    return *this;
}

template <typename T>
constexpr Rect<T> Rect<T>::setCenter(T x, T y) && {
    return std::move(this->setCenter(x, y));
}

template <typename T>
constexpr Rect<T>& Rect<T>::setCenter(const Vec2<T>& center) & {
    return setCenter(center.data[0], center.data[1]);
}

template <typename T>
constexpr Rect<T> Rect<T>::setCenter(const Vec2<T>& center) && {
    return std::move(this->setCenter(center.data[0], center.data[1]));
}

// --- setSize ---
template <typename T>
constexpr Rect<T>& Rect<T>::setSize(T w, T h) & {
    m_width = w;
    m_height = h;
    return *this;
}

template <typename T>
constexpr Rect<T> Rect<T>::setSize(T w, T h) && {
    return std::move(this->setSize(w, h));
}

template <typename T>
constexpr Rect<T>& Rect<T>::setSize(const Vec2<T>& size) & {
    return setSize(size.data[0], size.data[1]);
}

template <typename T>
constexpr Rect<T> Rect<T>::setSize(const Vec2<T>& size) && {
    return std::move(this->setSize(size.data[0], size.data[1]));
}

// --- scaleSize ---
template <typename T>
constexpr Rect<T>& Rect<T>::scaleSize(T s) & {
    m_width *= s;
    m_height *= s;
    return *this;
}

template <typename T>
constexpr Rect<T> Rect<T>::scaleSize(T s) && {
    return std::move(this->scaleSize(s));
}

template <typename T>
constexpr Rect<T>& Rect<T>::scaleSize(const Vec2<T>& s) & {
    m_width *= s.data[0];
    m_height *= s.data[1];
    return *this;
}

template <typename T>
constexpr Rect<T> Rect<T>::scaleSize(const Vec2<T>& s) && {
    return std::move(this->scaleSize(s));
}





template <typename T>
constexpr bool Rect<T>::contains(T x, T y) const {
    return (x >= m_min_x && x <= m_min_x + m_width && y >= m_min_y && y <= m_min_y + m_height);
}

template <typename T>
constexpr bool Rect<T>::contains(const Vec2<T>& point) const { return contains(point.data[0], point.data[1]); }

template <typename T>
constexpr bool Rect<T>::intersects(const Rect<T>& other) const {
    return (m_min_x < other.m_min_x + other.m_width) && (m_min_x + m_width > other.m_min_x) &&
           (m_min_y < other.m_min_y + other.m_height) && (m_min_y + m_height > other.m_min_y);
}

template <typename T>
constexpr bool Rect<T>::intersects(const Rect<T>& other, Rect<T>& out) const {
    T ix = std::max(m_min_x, other.m_min_x);
    T iy = std::max(m_min_y, other.m_min_y);
    T iw = std::min(m_min_x + m_width, other.m_min_x + other.m_width) - ix;
    T ih = std::min(m_min_y + m_height, other.m_min_y + other.m_height) - iy;
    
    if (iw > 0 && ih > 0) {
        out = Rect<T>(ix, iy, iw, ih);
        return true;
    }
    return false;
}