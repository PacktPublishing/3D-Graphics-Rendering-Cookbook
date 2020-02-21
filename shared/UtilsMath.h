#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/glm.hpp>

template <typename T>
T clamp(T v, T a, T b)
{
	if (v < a) return a;
	if (v > b) return b;
	return v;
}

inline glm::vec3 randVec()
{
	const float x = 10.0f * (float)rand() / (float)RAND_MAX - 5.0f;
	const float y = 10.0f * (float)rand() / (float)RAND_MAX - 5.0f;
	const float z = 10.0f * (float)rand() / (float)RAND_MAX - 5.0f;
	return glm::vec3(x, y, z);
}
