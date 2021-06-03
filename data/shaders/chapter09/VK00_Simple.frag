//
#version 460

//# i n c lude <data/shaders/chapter07/VK01.h>
//# i n clude <data/shaders/chapter07/VK01_VertCommon.h>

layout(location = 0) in vec3 v_worldNormal;
layout(location = 1) in vec4 v_worldPos;

layout(location = 0) out vec4 outColor;

void main()
{
	// world-space normal
	vec3 n = normalize(v_worldNormal);

	vec3 lightDir = normalize(vec3(-1.0, 1.0, -1.0));

	float NdotL = clamp( dot( n, lightDir ), 0.3, 1.0 );

	outColor = vec4( vec3(1.0, 1.0, 1.0) * NdotL, 1.0 );
//	outColor = vec4( 0.5*(n+vec3(1.0,1.0,1.0)), 1.0 );
//	outColor = vec4( n, 1.0 );
}
