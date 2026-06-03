#ifndef VECTOR_N_H
#define VECTOR_N_H

#include <cmath>
#include <cassert>
#include <cstddef>

#include "Geometry/vector_storage.hpp"

template<size_t N, typename T>
struct Vector:public VectorStorage<N,T> {
public:
    using VectorStorage<N, T>::data;

    // Constructors
    constexpr Vector();
    template<typename... Args>
    constexpr Vector(Args... args);
    template<typename U>
    explicit Vector(const Vector<N, U>& other);

    template<typename ExternalStruct>
    Vector(const ExternalStruct& other);

    // Basic Accessors (Optional but helpful)
    T& operator[](size_t i);
    const T& operator[](size_t i) const;

    // Arithmetic Operators
    Vector operator+(const Vector& rhs) const;
    Vector operator-(const Vector& rhs) const;
    constexpr Vector operator-() const;
    Vector operator*(T scalar) const;
    Vector operator/(T scalar) const;

    // Assignment Operators
    Vector& operator+=(const Vector& rhs);
    Vector& operator-=(const Vector& rhs);

    // Common Math
    T dot(const Vector& a) const;
    static T dot(const Vector& a, const Vector& b);
    T distanceSquared(const Vector& a);
    static T distanceSquared(const Vector& a, const Vector& b);
    
    T lengthSquared() const;
    float length() const;
    Vector<N, float> normalized() const;
};

// Aliases for convenience
template<typename T> using Vec2 = Vector<2, T>;
template<typename T> using Vec3 = Vector<3, T>;
template<typename T> using Vec4 = Vector<4, T>;

using Vec3f = Vector<3, float>;
using Vec3i = Vector<3, int>;

#include "vector.inl"

#endif