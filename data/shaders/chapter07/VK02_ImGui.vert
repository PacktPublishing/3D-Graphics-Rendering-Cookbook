//
#version 460

layout(location = 0) out vec2 uv;
layout(location = 1) out vec4 color;

struct ImDrawVert{ float x, y, u, v; uint color; };

layout(binding = 0) uniform  UniformBuffer { mat4   inMtx; } ubo;
layout(binding = 1) readonly buffer SBO    { ImDrawVert data[]; } sbo;
layout(binding = 2) readonly buffer IBO    { uint   data[]; } ibo;

void main()
{
	uint idx = ibo.data[gl_VertexIndex] + gl_BaseInstance;

	ImDrawVert v = sbo.data[idx];
	uv     = vec2(v.u, v.v);
	color  = unpackUnorm4x8(v.color);
	gl_Position = ubo.inMtx * vec4(v.x, v.y, 0.0, 1.0);
}
