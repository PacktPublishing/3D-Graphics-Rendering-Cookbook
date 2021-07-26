/** */
#version 460

layout (location = 0) in vec2 uv;
layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform UniformBuffer { uint width; uint height; } ubo;
layout (binding = 1) buffer Heads { uint heads[]; };

void main()
{
	uint fragIndex = uint(gl_FragCoord.y) * (ubo.width) + uint(gl_FragCoord.x);
	heads[fragIndex] = 0xFFFFFFFF; /// 0xCD00002F;

	// fake write to aux buffer
	outColor = vec4(1,1,1,1);
}
