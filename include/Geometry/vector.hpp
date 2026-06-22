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

    constexpr Vector();
    template<typename... Args> constexpr Vector(Args... args);
    template<typename U> explicit Vector(const Vector<N, U>& other);

    template<typename ExternalStruct>
    Vector(const ExternalStruct& other);


    // Basic Accessors (Optional but helpful)
    [[nodiscard]] T& operator[](size_t i);
    [[nodiscard]] const T& operator[](size_t i) const;

    [[nodiscard]] Vector operator+(const Vector& rhs) const;
    [[nodiscard]] Vector operator-(const Vector& rhs) const;
    [[nodiscard]] constexpr Vector operator-() const;
    [[nodiscard]] Vector operator*(T scalar) const;
    [[nodiscard]] Vector operator/(T scalar) const;

    Vector& operator+=(const Vector& rhs);
    Vector& operator-=(const Vector& rhs);



    [[nodiscard]] T dot(const Vector& a) const;
    [[nodiscard]] static T dot(const Vector& a, const Vector& b);
    [[nodiscard]] T distanceSquared(const Vector& a);
    [[nodiscard]] static T distanceSquared(const Vector& a, const Vector& b);
    
    [[nodiscard]] T lengthSquared() const;
    [[nodiscard]] float length() const;
    [[nodiscard]] Vector<N, float> normalized() const;
};

#include "vector.inl"

#endif