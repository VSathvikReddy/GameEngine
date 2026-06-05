#pragma once

#include <cmath>
#include <cassert>
#include <cstddef>

template<size_t N, typename T>
struct VectorStorage {
    T data[N];

    constexpr VectorStorage() : data{} {}

    template <typename... Args>
    constexpr VectorStorage(Args... args) : data{args...} {}
};

template<typename T>
struct VectorStorage<2, T> {
    union {
        T data[2];
        struct { T x, y; };
        struct { T r, g; };
        struct { T u, v; };
    };

    constexpr VectorStorage() : data{} {}

    template <typename... Args>
    constexpr VectorStorage(Args... args) : data{static_cast<T>(args)...} {}
};

template<typename T>
struct VectorStorage<3, T> {
    union {
        T data[3];
        struct { T x, y, z; };
        struct { T r, g, b; };
    };

    constexpr VectorStorage() : data{} {}

    template <typename... Args>
    constexpr VectorStorage(Args... args) : data{static_cast<T>(args)...} {}
};

template<typename T>
struct VectorStorage<4, T> {
    union {
        T data[4];
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
    };

    constexpr VectorStorage() : data{} {}

    template <typename... Args>
    constexpr VectorStorage(Args... args) : data{static_cast<T>(args)...} {}
};