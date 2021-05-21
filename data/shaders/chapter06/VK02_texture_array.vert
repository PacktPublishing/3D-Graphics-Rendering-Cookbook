//
#version 460

layout(location = 0) out vec2 out_uv;
layout(location = 1) flat out uint out_texIndex;

struct ImDrawVert
{
	float x, y, z, u, v;
};

layout(binding = 1) readonly buffer SBO { ImDrawVert data[]; } sbo;

layout(push_constant) uniform uPushConstant { vec2 position; uint textureIndex; } pc;

void main()
{
	uint idx = gl_VertexIndex;

	ImDrawVert v = sbo.data[idx];

	out_uv       = vec2(v.u, v.v);
	out_texIndex = pc.textureIndex;

	gl_Position = vec4(vec2(v.x, v.y) + pc.position, 0.0, 1.0);
}
