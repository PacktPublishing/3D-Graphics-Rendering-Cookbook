//
#version 460

#include <data/shaders/chapter05/GridParameters.h>

layout (location=0) out vec2 uv;

layout(binding = 0) uniform  UniformBuffer
{
	mat4 proj;
	mat4 view;
	mat4 model;
	float time;
} ubo;

void main()
{
	mat4 MVP = ubo.proj * ubo.view * ubo.model;

	int idx = indices[gl_VertexIndex];
	vec3 position = pos[idx] * gridSize;

	gl_Position = MVP * vec4(position, 1.0);
	uv = position.xz;
}
