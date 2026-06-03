#pragma once

#include <cmath>
#include <cassert>
#include <cstddef>

// 1. Define the base storage type
template<size_t N, typename T>
struct VectorStorage {
    T data[N];
};

template<typename T>
struct VectorStorage<2, T> {
    union {
        T data[2];
        struct { T x, y; };
        struct { T r, g; };
        struct { T u, v; };
    };
};

template<typename T>
struct VectorStorage<3, T> {
    union {
        T data[3];
        struct { T x, y, z; };
        struct { T r, g, b; };
    };
};

template<typename T>
struct VectorStorage<4, T> {
    union {
        T data[4];
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
    };
};