#include "GLSceneData.h"

static uint64_t getTextureHandleBindless(uint64_t idx, const std::vector<GLTexture>& textures)
{
	if (idx == INVALID_TEXTURE) return 0;

	return textures[idx].getHandleBindless();
}

GLSceneData::GLSceneData(
	const char* meshFile,
	const char* sceneFile,
	const char* materialFile)
{
	header_ = loadMeshData(meshFile, meshData_);
	loadScene(sceneFile);

	std::vector<std::string> textureFiles;
	loadMaterials(materialFile, materials_, textureFiles);

	for (const auto& f: textureFiles) {
		allMaterialTextures_.emplace_back(GL_TEXTURE_2D, f.c_str());
	}

	for (auto& mtl: materials_)
	{
		mtl.ambientOcclusionMap_ = getTextureHandleBindless(mtl.ambientOcclusionMap_, allMaterialTextures_);
		mtl.emissiveMap_ = getTextureHandleBindless(mtl.emissiveMap_, allMaterialTextures_);
		mtl.albedoMap_ = getTextureHandleBindless(mtl.albedoMap_, allMaterialTextures_);
		mtl.metallicRoughnessMap_ = getTextureHandleBindless(mtl.metallicRoughnessMap_, allMaterialTextures_);
		mtl.normalMap_ = getTextureHandleBindless(mtl.normalMap_, allMaterialTextures_);
	}
}

void GLSceneData::loadScene(const char* sceneFile)
{
	::loadScene(sceneFile, scene_);

	// prepare draw data buffer
	for (const auto& c: scene_.meshes_)
	{
		auto material = scene_.materialForNode_.find(c.first);
		if (material != scene_.materialForNode_.end())
		{
			shapes_.push_back(
				DrawData{
					.meshIndex = c.second,
					.materialIndex = material->second,
					.LOD = 0,
					.indexOffset = meshData_.meshes_[c.second].indexOffset,
					.vertexOffset = meshData_.meshes_[c.second].vertexOffset,
					.transformIndex = c.first
				});
		}
	}

	// force recalculation of all global transformations
	markAsChanged(scene_, 0);
	recalculateGlobalTransforms(scene_);
}
