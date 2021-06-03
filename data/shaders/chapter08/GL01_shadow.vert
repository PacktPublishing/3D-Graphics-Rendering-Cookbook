//
#version 460 core

#include <data/shaders/chapter08/GLBufferDeclarations.h>

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
};

vec3 getPosition(int i)
{
	return vec3(in_Vertices[i].p[0], in_Vertices[i].p[1], in_Vertices[i].p[2]);
}

void main()
{
	mat4 MVP = proj * view * in_ModelMatrices[gl_BaseInstance];

	gl_Position = MVP * vec4(getPosition(gl_VertexID), 1.0);
}
