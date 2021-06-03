//
#version 460 core

struct PerVertex
{
	vec4 color;
};

layout (location=0) in PerVertex vtx;

layout (location=0) out vec4 out_FragColor;

void main()
{
	out_FragColor = vtx.color;//vec4(1.0, 0.0, 0.0, 1.0);
};
