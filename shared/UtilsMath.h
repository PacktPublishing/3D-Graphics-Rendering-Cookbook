#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/glm.hpp>

namespace Math
{
	static constexpr float PI = 3.14159265359f;
	static constexpr float TWOPI = 6.28318530718f;
}

template <typename T>
T clamp(T v, T a, T b)
{
	if (v < a) return a;
	if (v > b) return b;
	return v;
}

inline float random01()
{
	return (float)rand() / (float)RAND_MAX;
}

inline float randomFloat(float min, float max)
{
	return min + (max- min) * random01();
}

inline glm::vec3 randomVec(const glm::vec3& min, const glm::vec3& max)
{
	return glm::vec3(randomFloat(min.x, max.x), randomFloat(min.y, max.y), randomFloat(min.z, max.z));
}

inline glm::vec3 randVec()
{
	return randomVec(glm::vec3(-5, -5, -5), glm::vec3(5, 5, 5));
}
