//
#version 460 core

struct node { uint idx; float xx, yy; };
layout (set = 0, binding = 0) buffer Values { node value[]; };

layout (set = 0, binding = 1) uniform UniformBuffer { float width; float height; } ubo;

void main()
{
	node n = value[gl_VertexIndex];
	gl_Position = vec4(2 * (vec2(n.xx / ubo.width, n.yy / ubo.height) - vec2(.5)), 0, 1);
	gl_PointSize = 1;
}
