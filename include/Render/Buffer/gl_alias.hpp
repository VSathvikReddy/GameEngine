#pragma once

#include <cstdint>

using ShaderID      = uint32_t;
using ShaderGPUID   = uint32_t;
constexpr ShaderID ERROR_SHADER     = 0;
constexpr ShaderID NO_SHADER        = 0;


using TextureID     = uint32_t;
using TextureGPUID  = uint32_t;
constexpr TextureID ERROR_TEXTURE   = 0;



using VaoID         = uint32_t;
using VaoGPUID      = uint32_t;

using VboID         = uint32_t;
using VboGPUID      = uint32_t;

using EboID         = uint32_t;
using EboGPUID      = uint32_t;

using UboID         = uint32_t;
using UboGPUID      = uint32_t;