//
#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
	float tesselationScale;
};

struct Vertex
{
	float p[3];
	float tc[2];
};

layout(std430, binding = 1) restrict readonly buffer Vertices
{
	Vertex in_Vertices[];
};

layout(std430, binding = 2) restrict readonly buffer Matrices
{
	mat4 in_Model[];
};

vec3 getPosition(int i)
{
	return vec3(in_Vertices[i].p[0], in_Vertices[i].p[1], in_Vertices[i].p[2]);
}

vec2 getTexCoord(int i)
{
	return vec2(in_Vertices[i].tc[0], in_Vertices[i].tc[1]);
}

layout (location=0) out vec2 uv_in;
layout (location=1) out vec3 worldPos_in;

void main()
{
	mat4 MVP = proj * view * in_Model[gl_DrawID];

	vec3 pos = getPosition(gl_VertexID);
	gl_Position = MVP * vec4(pos, 1.0);

	uv_in = getTexCoord(gl_VertexID);
	worldPos_in = ( in_Model[gl_DrawID] * vec4(pos, 1.0) ).xyz;
}
