//
#version 460

#include <data/shaders/chapter06/mesh_common.inc>

layout(location = 0) in VertexData vtx;
layout(location = 0) out vec4 out_Color;

layout(binding = 3) uniform sampler2D texture0;

vec3 hue2rgb(float hue)
{
	float h = fract(hue);
	float r = abs(h * 6 - 3) - 1;
	float g = 2 - abs(h * 6 - 2);
	float b = 2 - abs(h * 6 - 4);
	return clamp(vec3(r,g,b), vec3(0), vec3(1));
}

void main()
{
	const vec3 cameraPos = vec3(0, 0, -12);
	// N dot L
	float dd = dot(normalize(vtx.norm.xyz), normalize(vtx.pos.xyz - cameraPos));
	float intensity = 2.0 * clamp(dd, 0.5, 1);
	out_Color = vec4(intensity * hue2rgb(vtx.tc.x), 1);
}
