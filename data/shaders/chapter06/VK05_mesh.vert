//
#version 460

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec2 tc;
layout(location = 2) out vec3 normal;

layout(binding = 0) uniform UniformBuffer
{
	mat4 mvp;
	mat4 mv;
	mat4 m;
	vec4 cameraPos;
} ubo;

struct VertexData
{
	vec4 pos;
	vec4 n;
	vec4 tc;
};

layout(binding=1) readonly buffer Vertices { VertexData data[]; } in_Vertices;
layout(binding=2) readonly buffer Indices { uint data[]; } in_Indices;

void main()
{
	VertexData vtx = in_Vertices.data[in_Indices.data[gl_VertexIndex]];

	tc = vtx.tc.xy;

	mat3 normalMatrix = transpose( inverse(mat3(ubo.m)) );
	normal = normalMatrix  * vtx.n.xyz;
	worldPos = (ubo.m * vtx.pos).xyz;

	gl_Position = ubo.mvp * vtx.pos;
}
