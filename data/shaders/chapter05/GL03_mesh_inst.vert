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

layout (location=0) out vec2 uv;
layout (location=1) out vec3 wpos;

void main()
{
	mat4 MVP = proj * view * in_Model[gl_InstanceID];

	gl_Position = MVP * vec4(pos, 1.0);

	wpos = pos;

	uv = vec2(0.5, 0.5);
}
