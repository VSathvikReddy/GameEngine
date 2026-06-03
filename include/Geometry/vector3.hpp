#ifndef VECTOR_3_H
#define VECTOR_3_H

#include "Geometry/vector.hpp"


template<typename T>
using Vec3 = Vector<3,T>;

using Vec3f  = Vec3<float>;
using Vec3d  = Vec3<double>;

using Vec3i  = Vec3<int>;
using Vec3l  = Vec3<long>;


#endif