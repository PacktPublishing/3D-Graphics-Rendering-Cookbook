//
#version 460

layout(location = 0) in vec3 worldPos;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(normalize(worldPos), 1.0);
}
