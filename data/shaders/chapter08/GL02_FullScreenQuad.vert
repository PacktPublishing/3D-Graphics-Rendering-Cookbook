//
#version 460 core

layout (location=0) out vec2 uv;

void main()
{
	float u = float( ((uint(gl_VertexID) + 2u) / 3u) % 2u );
	float v = float( ((uint(gl_VertexID) + 1u) / 3u) % 2u );

	gl_Position = vec4(-1.0+u*2.0, -1.0+v*2.0, 0.0, 1.0);
	uv = vec2(u, v);
}
