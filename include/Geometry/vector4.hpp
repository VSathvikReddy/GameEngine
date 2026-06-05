#ifndef VECTOR_4_H
#define VECTOR_4_H

#include "Geometry/vector.hpp"


template<typename T>
using Vec4 = Vector<4,T>;

using Vec4f  = Vec4<float>;
using Vec4d  = Vec4<double>;

using Vec4i  = Vec4<int>;
using Vec4l  = Vec4<long>;


#endif