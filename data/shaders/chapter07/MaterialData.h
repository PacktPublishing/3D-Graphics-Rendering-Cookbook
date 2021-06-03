//

struct MaterialData
{
	vec4 emissiveColor_;
	vec4 albedoColor_;
	vec4 roughness_;

	float transparencyFactor_;
	float alphaTest_;
	float metallicFactor_;

	uint  flags_;

	uint64_t ambientOcclusionMap_;
	uint64_t emissiveMap_;
	uint64_t albedoMap_;
	uint64_t metallicRoughnessMap_;
	uint64_t normalMap_;
	uint64_t opacityMap_;
};
