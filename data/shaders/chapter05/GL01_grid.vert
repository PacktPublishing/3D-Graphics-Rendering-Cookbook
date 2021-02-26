//
#version 460 core

#include <data/shaders/chapter04/GLBufferDeclarations.h>
#include <data/shaders/chapter05/GridParameters.h>

layout (location=0) out vec2 uv;

void main()
{
	mat4 MVP = proj * view;

	int idx = indices[gl_VertexID];
	vec3 position = pos[idx] * gridSize;

	gl_Position = MVP * vec4(position, 1.0);
	uv = position.xz;
}
