//
#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
};

layout(std430, binding = 2) restrict readonly buffer Matrices
{
	mat4 in_Model[];
};

layout (location=0) in vec3 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 tc;

layout (location=0) out vec2 uv;

void main()
{
	mat4 MVP = proj * view * in_Model[gl_DrawID];

	gl_Position = MVP * vec4(pos, 1.0);

	uv = tc;
}
