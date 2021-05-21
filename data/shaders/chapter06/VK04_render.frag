//
#version 460

#include <data/shaders/chapter06/mesh_common.inc>

layout(location = 0) in VertexData vtx;
layout(location = 0) out vec4 out_Color;

layout(binding = 3) uniform sampler2D texture0;

void main()
{
	const vec3 cameraPos = vec3(0, 0, -12);

	// N dot L
	float dd = dot(normalize(vtx.norm.xyz), normalize(vtx.pos.xyz - cameraPos));

	float intensity = 2.0 * clamp(dd, 0.5, 1);
	vec3 color = texture(texture0, vec2(8,1)*vtx.tc.xy).xyz;
	out_Color = vec4(intensity * color, 1 );
}
