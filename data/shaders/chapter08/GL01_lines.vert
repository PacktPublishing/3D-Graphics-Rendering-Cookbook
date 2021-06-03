//
#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
};

struct Vertex
{
	float p[3];
	float c[4];
};

layout(std430, binding = 1) restrict readonly buffer Vertices
{
	Vertex in_Vertices[];
};

vec3 getPosition(int i)
{
	return vec3(in_Vertices[i].p[0], in_Vertices[i].p[1], in_Vertices[i].p[2]);
}

vec4 getColor(int i)
{
	return vec4(in_Vertices[i].c[0], in_Vertices[i].c[1], in_Vertices[i].c[2], in_Vertices[i].c[3]);
}

struct PerVertex
{
	vec4 color;
};

layout (location=0) out PerVertex vtx;

void main()
{
	gl_Position = proj * view * vec4(getPosition(gl_VertexID), 1.0);

	vtx.color = getColor(gl_VertexID);
}
