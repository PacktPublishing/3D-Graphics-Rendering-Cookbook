//
#version 460

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 uv;

layout(binding = 0) uniform UniformBuffer
{
	mat4 mvp;
} ubo;

struct VertexData
{
	float x, y, z;
	float u, v;
};

layout(binding=1) readonly buffer Vertices { VertexData data[]; } in_Vertices;
layout(binding=2) readonly buffer Indices { uint data[]; } in_Indices;

void main()
{
	uint idx = in_Indices.data[gl_VertexIndex];

	VertexData vtx = in_Vertices.data[idx];

	vec3 pos = vec3(vtx.x, vtx.y, vtx.z);

	gl_Position = ubo.mvp * vec4(pos, 1.0);

	fragColor = pos;
	uv = vec2(vtx.u, vtx.v);
}
