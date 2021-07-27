//
#version 460

#extension GL_EXT_nonuniform_qualifier : require

layout (early_fragment_tests) in;

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

layout(binding = 6) readonly buffer ShadowBO  { mat4 lightProj; mat4 lightView; uint width; uint height; } shadow_bo;

struct TransparentFragment {
	vec4 color;
	float depth;
	uint next;
};

layout (binding = 7) buffer Atomic { uint numFragments; };
layout (binding = 8) buffer Heads { uint heads[]; };
layout (binding = 9) buffer Lists { TransparentFragment fragments[]; };

layout(binding = 10) uniform samplerCube texEnvMap;
layout(binding = 11) uniform samplerCube texEnvMapIrradiance;
layout(binding = 12) uniform sampler2D   texBRDF_LUT;

layout(binding = 13) uniform sampler2D shadowMap;

// All 2D textures for all of the materials
layout(binding = 14) uniform sampler2D textures[];

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

	// world-space normal
	vec3 n = normalize(v_worldNormal);

	// normal mapping: skip missing normal maps
	if (length(normalSample) > 0.5)
		n = perturbNormal(n, normalize(ubo.cameraPos.xyz - v_worldPos.xyz), normalSample, uvw.xy);

	// image-based lighting (diffuse only)
	vec3 f0 = vec3(0.04);
	vec3 diffuseColor = albedo.rgb * (vec3(1.0) - f0);
	vec3 diffuse = texture(texEnvMapIrradiance, n.xyz).rgb * diffuseColor;

	// some ad hoc environment reflections for transparent objects
	vec3 v = normalize(ubo.cameraPos.xyz - v_worldPos.xyz);
	vec3 reflection = reflect(v, n);
	vec3 colorRefl = texture(texEnvMap, reflection).rgb;

	outColor = vec4(diffuse + colorRefl, 1.0);

	float alpha = clamp(albedo.a, 0.0, 1.0) * md.transparencyFactor_;
	bool isTransparent = alpha < 0.99;
	if (isTransparent && gl_HelperInvocation == false)
	{
		if (alpha > 0.01)
		{
			uint index = atomicAdd(numFragments, 1);
			const uint maxOITfragments = 16 * 1024 * 1024;
			if (index < maxOITfragments)
			{
				uint fragIndex = uint(gl_FragCoord.y) * (shadow_bo.width)  + uint(gl_FragCoord.x);
				uint prevIndex = atomicExchange(heads[fragIndex], index);
				fragments[index].color = vec4(outColor.rgb, alpha);
				fragments[index].depth = gl_FragCoord.z;
				fragments[index].next  = prevIndex;
			}
		}
	}

 	outColor = vec4(0, 0, 0, 0);
}
