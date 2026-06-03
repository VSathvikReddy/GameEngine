#ifndef VECTOR_2_H
#define VECTOR_2_H

#include "Geometry/vector.hpp"

#include <cmath>
#include <cassert>


template<typename T>
using Vec2 = Vector<2,T>;

using Vec2f  = Vec2<float>;
using Vec2d  = Vec2<double>;

using Vec2i  = Vec2<int>;
using Vec2l  = Vec2<long>;


#endif