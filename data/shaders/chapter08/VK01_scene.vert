//
#version 460

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec4 shadowCoord;
layout(location = 2) out vec2 uv;

layout(binding = 0) uniform UniformBuffer
{
	mat4 mvp;
	mat4 model;
	mat4 lightMVP;
	vec4 cameraPos;
	vec4 lightPos;
	float meshScale;
} ubo;

struct VertexData
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
};

layout(binding=1) readonly buffer Vertices { VertexData data[]; } in_Vertices;
layout(binding=2) readonly buffer Indices  { uint data[]; } in_Indices;

// Vulkan's Z is in 0..1
const mat4 scaleBias = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0);

void main()
{
	uint idx = in_Indices.data[gl_VertexIndex];

	VertexData vtx = in_Vertices.data[idx];

	vec4 pos = vec4(ubo.meshScale * vec3(vtx.x, vtx.y, vtx.z), 1.0);

	worldPos = (ubo.model * pos).xyz;
	shadowCoord = scaleBias * ubo.lightMVP * vec4(worldPos, 1.0);
	uv = vec2(vtx.u, vtx.v);

	gl_Position = ubo.mvp * pos;
}
