#pragma once

#include <cstdint>
#include "shared/scene/vec4.h"

#include <string>
#include <vector>

enum MaterialFlags
{
	sMaterialFlags_CastShadow = 0x1,
	sMaterialFlags_ReceiveShadow = 0x2,
};

struct PACKED_STRUCT MaterialDescription final
{
	gpuvec4 emissiveColor_ = { 0.0f, 0.0f, 0.0f, 0.0f};
	gpuvec4 albedoColor_   = { 1.0f, 1.0f, 1.0f, 1.0f };
	// UV anisotropic roughness (isotropic lighting models use only the first value). ZW values are ignored
	gpuvec4 roughness_     = { 1.0f, 1.0f, 0.0f, 0.0f };
	float transparencyFactor_ = 1.0f;
	float alphaTest_          = 0.0f;
	float metallicFactor_     = 1.0f;
	uint32_t flags_ = sMaterialFlags_CastShadow | sMaterialFlags_ReceiveShadow;
	// maps
	uint64_t ambientOcclusionMap_ = 0xFFFFFFFF;
	uint64_t emissiveMap_ = 0xFFFFFFFF;
	uint64_t albedoMap_ = 0xFFFFFFFF;
	/// Occlusion (R), Roughness (G), Metallic (B) https://github.com/KhronosGroup/glTF/issues/857
	uint64_t metallicRoughnessMap_ = 0xFFFFFFFF;
	uint64_t normalMap_ = 0xFFFFFFFF;
	uint64_t opacityMap_ = 0xFFFFFFFF;
};

static_assert(sizeof(MaterialDescription) % 16 == 0, "MaterialDescription should be padded to 16 bytes");

void saveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files);
void loadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files);
