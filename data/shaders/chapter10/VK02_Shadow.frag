//
#version 460

#extension GL_EXT_nonuniform_qualifier : require

#include <data/shaders/chapter07/VK01.h>
#include <data/shaders/chapter07/VK01_VertCommon.h>
#include <data/shaders/chapter07/AlphaTest.h>

layout(location = 0) in vec3 uvw;
layout(location = 1) in vec3 v_worldNormal;
layout(location = 2) in vec4 v_worldPos;
layout(location = 3) in flat uint matIdx;
layout(location = 4) in vec4 v_shadowCoord;

layout(location = 0) out vec4 outColor;

// Buffer with PBR material coefficients
layout(binding = 4) readonly buffer MatBO  { MaterialData data[]; } mat_bo;

layout(binding = 6) readonly buffer ShadowBO  { mat4 lightProj; mat4 lightView; } shadow_bo;

layout(binding = 7) uniform samplerCube texEnvMap;
layout(binding = 8) uniform samplerCube texEnvMapIrradiance;
layout(binding = 9) uniform sampler2D   texBRDF_LUT;

layout(binding = 10) uniform sampler2D shadowMap;

// All 2D textures for all of the materials
layout(binding = 11) uniform sampler2D textures[];

#include <data/shaders/chapter06/PBR.sp>

float PCF(int kernelSize, vec2 shadowCoord, float depth)
{
	float size = 1.0 / float( textureSize(shadowMap, 0 ).x );
	float shadow = 0.0;
	int range = kernelSize / 2;
	for ( int v=-range; v<=range; v++ ) for ( int u=-range; u<=range; u++ )
		shadow += (depth >= texture( shadowMap, shadowCoord + size * vec2(u, v) ).r) ? 1.0 : 0.0;
	return shadow / (kernelSize * kernelSize);
}

float shadowFactor(vec4 shadowCoord)
{
	if (shadow_bo.lightProj[3][3] == 0.0)
		return 1.0;

	vec4 shadowCoords4 = shadowCoord / shadowCoord.w;

	if (shadowCoords4.z > -1.0 && shadowCoords4.z < 1.0)
	{
		float depthBias = -0.001;
		float shadowSample = PCF( 13, shadowCoords4.xy, shadowCoords4.z + depthBias );
		return mix(1.0, 0.3, shadowSample);
	}

	return 1.0; 
}

void main()
{

	MaterialData md = mat_bo.data[matIdx];

	vec4 emission = vec4(0,0,0,0); // md.emissiveColor_;
	vec4 albedo = md.albedoColor_;
	vec3 normalSample = vec3(0.0, 0.0, 0.0);

	const int INVALID_HANDLE = 2000;

	// fetch albedo
	if (md.albedoMap_ < INVALID_HANDLE)
	{
		uint texIdx = uint(md.albedoMap_);
		albedo = texture(textures[nonuniformEXT(texIdx)], uvw.xy);
	}
	if (md.normalMap_ < INVALID_HANDLE)
	{
		uint texIdx = uint(md.normalMap_);
		normalSample = texture(textures[nonuniformEXT(texIdx)], uvw.xy).xyz;
	}

	runAlphaTest(albedo.a, md.alphaTest_);

	// world-space normal
	vec3 n = normalize(v_worldNormal);

	// normal mapping: skip missing normal maps
	if (length(normalSample) > 0.5)
		n = perturbNormal(n, normalize(ubo.cameraPos.xyz - v_worldPos.xyz), normalSample, uvw.xy);

	// image-based lighting (diffuse only)
	vec3 f0 = vec3(0.04);
	vec3 diffuseColor = albedo.rgb * (vec3(1.0) - f0);
	vec3 diffuse = texture(texEnvMapIrradiance, n.xyz).rgb * diffuseColor;

	outColor = vec4( diffuse * shadowFactor(v_shadowCoord), 1.0 );
}
