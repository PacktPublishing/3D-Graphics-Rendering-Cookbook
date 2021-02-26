//
#version 460 core

layout (location=0) in vec3 dir;

layout (location=0) out vec4 outColor;

layout (binding=1) uniform samplerCube texture1;

void main()
{
	outColor = texture(texture1, dir);
};
