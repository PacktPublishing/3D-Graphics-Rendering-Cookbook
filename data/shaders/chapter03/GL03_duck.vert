//
#version 460 core

#include <data/shaders/chapter03/GLBufferDeclarations.h>

vec3 getPosition(int i)
{
	return vec3(in_Vertices[i].p[0], in_Vertices[i].p[1], in_Vertices[i].p[2]);
}

vec3 getNormal(int i)
{
	return vec3(in_Vertices[i].n[0], in_Vertices[i].n[1], in_Vertices[i].n[2]);
}

vec2 getTexCoord(int i)
{
	return vec2(in_Vertices[i].tc[0], in_Vertices[i].tc[1]);
}

struct PerVertex
{
	vec2 uv;
	vec3 normal;
	vec3 worldPos;
};

layout (location=0) out PerVertex vtx;

void main()
{
	vec3 pos = getPosition(gl_VertexID);
	gl_Position = MVP * vec4(pos, 1.0);

	mat3 normalMatrix = mat3(transpose(inverse(model)));

	vtx.uv = getTexCoord(gl_VertexID);
	vtx.normal = getNormal(gl_VertexID) * normalMatrix;
	vtx.worldPos = (model * vec4(pos, 1.0)).xyz;
}
