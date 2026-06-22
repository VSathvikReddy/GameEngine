#pragma once

#include "vec2.hpp"

template<typename DataType>
class Rectangle{


private:
    Vec2<DataType> m_top_left{};
    Vec2<DataType> dimensions{};
};