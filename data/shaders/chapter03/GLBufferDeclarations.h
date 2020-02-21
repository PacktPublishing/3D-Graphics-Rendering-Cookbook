//

layout(std140, binding = 0) uniform PerFrameData
{
	uniform mat4 model;
	uniform mat4 MVP;
	uniform vec4 cameraPos;
};

struct Vertex
{
	float p[3];
	float n[3];
	float tc[2];
};

layout(std430, binding = 1) restrict readonly buffer Vertices
{
	Vertex in_Vertices[];
};
