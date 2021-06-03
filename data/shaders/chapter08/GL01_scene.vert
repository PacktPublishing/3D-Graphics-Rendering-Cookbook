//
#version 460 core

#include <data/shaders/chapter08/GLBufferDeclarations.h>

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
	mat4 light;
	vec4 cameraPos;
	vec4 lightAngles;
	vec4 lightPos;
};

vec3 getPosition(int i)
{
	return vec3(in_Vertices[i].p[0], in_Vertices[i].p[1], in_Vertices[i].p[2]);
}

vec2 getTexCoord(int i)
{
	return vec2(in_Vertices[i].tc[0], in_Vertices[i].tc[1]);
}

struct PerVertex
{
	vec2 uv;
	vec4 shadowCoord;
	vec3 worldPos;
};

layout (location=0) out PerVertex vtx;

// OpenGL's Z is in -1..1
const mat4 scaleBias = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0 );

void main()
{
	mat4 model = in_ModelMatrices[gl_BaseInstance];
	mat4 MVP = proj * view * model;

	vec3 pos = getPosition(gl_VertexID);

	gl_Position = MVP * vec4(pos, 1.0);

	vtx.uv = getTexCoord(gl_VertexID);
	vtx.shadowCoord = scaleBias * light * model * vec4(pos, 1.0);	
	vtx.worldPos = (model * vec4(pos, 1.0)).xyz;
}
