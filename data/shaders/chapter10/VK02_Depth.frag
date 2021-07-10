//
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 uvw;
layout(location = 1) in vec3 v_worldNormal;

void main()
{
	vec3 n = normalize(v_worldNormal);

	vec3 lightDir = normalize(vec3(-1.0, 1.0, 0.1));

	float NdotL = clamp( dot( n, lightDir ), 0.0, 1.0 );

	outColor = vec4( NdotL * vec3(1, 1, 1), 1.0 );

}
