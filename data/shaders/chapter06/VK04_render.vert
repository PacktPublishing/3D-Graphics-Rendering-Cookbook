//
#version 460

#include <data/shaders/chapter06/mesh_common.inc>

layout(location = 0) out VertexData out_Vertex;

layout(binding = 0) uniform UniformBuffer
{
	mat4 mvp;
} ubo;

layout(binding=1) readonly buffer Vertices { VertexData data[]; } in_Vertices;
layout(binding=2) readonly buffer Indices  { uint data[];       } in_Indices;

void main()
{
	uint idx = in_Indices.data[gl_VertexIndex];

   out_Vertex = in_Vertices.data[idx];
	gl_Position = ubo.mvp * out_Vertex.pos;
}
