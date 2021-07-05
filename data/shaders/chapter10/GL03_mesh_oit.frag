//
#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

#include <data/shaders/chapter07/MaterialData.h>

layout (early_fragment_tests) in;

#include <data/shaders/chapter10/GLBufferDeclarations.h>

layout(std430, binding = 2) restrict readonly buffer Materials
{
	MaterialData in_Materials[];
};

layout (location=0) in vec2 v_tc;
layout (location=1) in vec3 v_worldNormal;
layout (location=2) in vec3 v_worldPos;
layout (location=3) in flat uint matIdx;

layout (location=0) out vec4 out_FragColor;

layout (binding = 5) uniform samplerCube texEnvMap;
layout (binding = 6) uniform samplerCube texEnvMapIrradiance;
layout (binding = 7) uniform sampler2D texBRDF_LUT;

#include <data/shaders/chapter07/AlphaTest.h>
#include <data/shaders/chapter06/PBR.sp>

struct TransparentFragment
{
	vec4 color;
	float depth;
	uint next;
};

layout (binding = 0, r32ui) uniform uimage2D heads;
layout (binding = 0, offset = 0) uniform atomic_uint numFragments;

layout (std430, binding = 3) buffer Lists
{
    TransparentFragment Fragments[];
};

void main()
{
	MaterialData mtl = in_Materials[matIdx];

	vec4 albedo = mtl.albedoColor_;
	vec3 normalSample = vec3(0.0, 0.0, 0.0);

	// fetch albedo
	if (mtl.albedoMap_ > 0)
		albedo = texture( sampler2D(unpackUint2x32(mtl.albedoMap_)), v_tc);
	if (mtl.normalMap_ > 0)
		normalSample = texture( sampler2D(unpackUint2x32(mtl.normalMap_)), v_tc).xyz;

	// world-space normal
	vec3 n = normalize(v_worldNormal);

	// normal mapping: skip missing normal maps
	if (length(normalSample) > 0.5)
		n = perturbNormal(n, normalize(cameraPos.xyz - v_worldPos.xyz), normalSample, v_tc);

	// image-based lighting (diffuse only)
	vec3 f0 = vec3(0.04);
	vec3 diffuseColor = albedo.rgb * (vec3(1.0) - f0);
	vec3 diffuse = texture(texEnvMapIrradiance, n.xyz).rgb * diffuseColor;
	// some ad hoc environment reflections for transparent objects
	vec3 v = normalize(cameraPos.xyz - v_worldPos);
	vec3 reflection = reflect(v, n);
	vec3 colorRefl = texture(texEnvMap, reflection).rgb;

	out_FragColor = vec4(diffuse + colorRefl, 1.0);

	// Order-Independent Transparency: https://fr.slideshare.net/hgruen/oit-and-indirect-illumination-using-dx11-linked-lists
	float alpha = clamp(albedo.a, 0.0, 1.0) * mtl.transparencyFactor_;
	bool isTransparent = alpha < 0.99;
	if (isTransparent && gl_HelperInvocation == false)
	{
		if (alpha > 0.01)
		{
			uint index = atomicCounterIncrement(numFragments);
			const uint maxOITfragments = 16*1024*1024;
			if (index < maxOITfragments)
			{
				uint prevIndex = imageAtomicExchange(heads, ivec2(gl_FragCoord.xy), index);
				Fragments[index].color = vec4(out_FragColor.rgb, alpha);
				Fragments[index].depth = gl_FragCoord.z;
				Fragments[index].next  = prevIndex;
			}
		}
	}
};
