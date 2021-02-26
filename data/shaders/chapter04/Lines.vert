//
#version 460

layout(location = 0) out vec4 lineColor;

layout(binding=0) uniform UBO {
	mat4 inMtx;
	float time;
} ubo;

struct DrawVert
{
	float x, y, z;
	float r, g, b, a;
};

layout(binding=1) readonly buffer SBO { DrawVert data[]; } sbo;

void main()
{
	DrawVert v = sbo.data[gl_VertexIndex];

	gl_Position = ubo.inMtx * vec4(v.x, v.y, v.z, 1.0);
	lineColor = vec4(v.r, v.g, v.b, v.a);
}
