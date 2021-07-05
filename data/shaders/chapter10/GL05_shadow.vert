//
#version 460 core

#include <data/shaders/chapter10/GLBufferDeclarations.h>

layout(std430, binding = 1) restrict readonly buffer Matrices
{
	mat4 in_Model[];
};

layout (location=0) in vec3 in_Vertex;

void main()
{
	mat4 model = in_Model[gl_BaseInstance >> 16];

	gl_Position = proj * view * model * vec4(in_Vertex, 1.0);
}
