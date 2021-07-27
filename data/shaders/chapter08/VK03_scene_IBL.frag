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

layout(location = 0) out vec4 outColor;

// Buffer with PBR material coefficients
layout(binding = 4) readonly buffer MatBO  { MaterialData data[]; } mat_bo;

layout(binding = 6) uniform samplerCube texEnvMap;
layout(binding = 7) uniform samplerCube texEnvMapIrradiance;
layout(binding = 8) uniform sampler2D   texBRDF_LUT;
//layout(binding = 9) uniform sampler2D texMetalRoughness;

// All 2D textures for all of the materials
layout(binding = 9) uniform sampler2D textures[];

#include <data/shaders/chapter06/PBR.sp>

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
	{
		n = perturbNormal(n, normalize(ubo.cameraPos.xyz - v_worldPos.xyz), normalSample, uvw.xy);
	}

	// image-based lighting (diffuse only)
	vec3 f0 = vec3(0.04);
	vec3 diffuseColor = albedo.rgb * (vec3(1.0) - f0);
	vec3 diffuse = texture(texEnvMapIrradiance, n.xyz).rgb * diffuseColor;

	outColor = vec4( diffuse, 1.0 );
}
