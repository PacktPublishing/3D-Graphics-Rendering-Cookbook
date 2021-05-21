//
#version 460

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 tc;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 out_FragColor;

layout(binding = 0) uniform UniformBuffer
{
	mat4 mvp;
	mat4 mv;
	mat4 m;
	vec4 cameraPos;
} ubo;

layout(binding = 3) uniform sampler2D texAO;
layout(binding = 4) uniform sampler2D texEmissive;
layout(binding = 5) uniform sampler2D texAlbedo;
layout(binding = 6) uniform sampler2D texMetalRoughness;
layout(binding = 7) uniform sampler2D texNormal;

layout(binding = 8) uniform samplerCube texEnvMap;
layout(binding = 9) uniform samplerCube texEnvMapIrradiance;

layout(binding = 10) uniform sampler2D texBRDF_LUT;

#include <data/shaders/chapter06/PBR.sp>

void main()
{
//	out_FragColor = vec4( texture(texBRDF_LUT, tc).xyz, 1.0 );
//	out_FragColor = vec4( (normal + vec3(1.0)) * 0.5, 1.0 );

	vec4 Kao = texture(texAO, tc);
	vec4 Ke  = texture(texEmissive, tc);
	vec4 Kd  = texture(texAlbedo, tc);
	vec2 MeR = texture(texMetalRoughness, tc).yz;

	vec3 normalSample = texture(texNormal, tc).xyz;

	// world-space normal
	vec3 n = normalize(normal);

	// normal mapping
	n = perturbNormal(n, normalize(ubo.cameraPos.xyz - worldPos), normalSample, tc);

	vec4 mrSample = texture(texMetalRoughness, tc);

	PBRInfo pbrInputs;
	Ke.rgb = SRGBtoLINEAR(Ke).rgb;
	// image-based lighting
	vec3 color = calculatePBRInputsMetallicRoughness(Kd, n, ubo.cameraPos.xyz, worldPos, mrSample, pbrInputs);
	// one hardcoded light source
	color += calculatePBRLightContribution( pbrInputs, normalize(vec3(-1.0, -1.0, -1.0)), vec3(1.0) );
	// ambient occlusion
	color = color * ( Kao.r < 0.01 ? 1.0 : Kao.r );
	// emissive
	color = pow( Ke.rgb + color, vec3(1.0/2.2) );

	out_FragColor = vec4(color, 1.0);

	// test cube map
//	vec3 v = normalize(ubo.cameraPos.xyz - worldPos);
//	vec3 reflection = -normalize(reflect(v, n));
//	out_FragColor = texture( texEnvMap, reflection );
//	out_FragColor = texture( texEnvMap, reflection * vec3(-1,-1,1) );

//	out_FragColor = vec4(tc, 0.0, 1.0);
//	out_FragColor = vec4((n + vec3(1.0))*0.5, 1.0);
//	out_FragColor = Kao;
//	out_FragColor = Ke;
//	out_FragColor = Kd;
//	out_FragColor = vec4(MeR, 0.0, 1.0);

}
