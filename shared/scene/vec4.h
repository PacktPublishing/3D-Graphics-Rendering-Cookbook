#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string.h>

#ifdef __GNUC__
#define PACKED_STRUCT __attribute__((packed,aligned(1)))
#else
#define PACKED_STRUCT
#endif

using glm::vec2;
using glm::vec4;

struct PACKED_STRUCT gpuvec4
{
	float x, y, z, w;

	gpuvec4() = default;
	explicit gpuvec4(float v): x(v), y(v), z(v), w(v) {}
	gpuvec4(float a, float b, float c, float d): x(a), y(b), z(c), w(d) {}
	explicit gpuvec4(const vec4& v): x(v.x), y(v.y), z(v.z), w(v.w) {}
};

struct PACKED_STRUCT gpumat4
{
	float data_[16];

	gpumat4() = default;
	explicit gpumat4(const glm::mat4& m)  { memcpy(data_, glm::value_ptr(m), 16 * sizeof(float)); }
};
