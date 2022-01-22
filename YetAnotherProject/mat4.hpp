#ifndef MAT4_INCLUDED
#define MAT4_INCLUDED

#pragma once

#include <array>
#include "vec.hpp"

using mat4f = std::array<std::array<float, 4>, 4>;

namespace mat {

	mat4f zeroed();

	mat4f identity();

	mat4f multiply(const mat4f& a, const mat4f& b);

	mat4f ortho(float width, float height, float nearPlane, float farPlane);

	mat4f ortho(const vec2f& dimension, const vec2f& near_far_planes);
}

#endif