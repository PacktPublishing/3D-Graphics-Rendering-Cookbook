#include <memory>

#include "GLSceneDataLazy.h"
#include <stb/stb_image.h>

static uint64_t getTextureHandleBindless(uint64_t idx, const std::vector<std::shared_ptr<GLTexture>>& textures)
{
	if (idx == INVALID_TEXTURE) return 0;

	return textures[idx]->getHandleBindless();
}

GLSceneDataLazy::GLSceneDataLazy(
	const char* meshFile,
	const char* sceneFile,
	const char* materialFile)
{
	header_ = loadMeshData(meshFile, meshData_);
	loadScene(sceneFile);
	loadMaterials(materialFile, materialsLoaded_, textureFiles_);

	// apply a dummy textures to everything
	for (const auto& f: textureFiles_) {
		allMaterialTextures_.emplace_back(dummyTexture_);
	}

	updateMaterials();

	loadedFiles_.reserve(textureFiles_.size());

	taskflow_.for_each_index(0u, (uint32_t)textureFiles_.size(), 1u, [this](int idx)
		{
			int w, h;
			const uint8_t* img = stbi_load(this->textureFiles_[idx].c_str(), &w, &h, nullptr, STBI_rgb_alpha);
			if (img)
			{
				std::lock_guard lock(loadedFilesMutex_);
				loadedFiles_.emplace_back(LoadedImageData { idx, w, h, img });
			}
		}
	);

	executor_.run(taskflow_);
}

bool GLSceneDataLazy::uploadLoadedTextures()
{
	LoadedImageData data;

	{
		std::lock_guard lock(loadedFilesMutex_);

		if (loadedFiles_.empty())
			return false;

		data = loadedFiles_.back();

		loadedFiles_.pop_back();
	}

	allMaterialTextures_[data.index_] = std::make_shared<GLTexture>(data.w_, data.h_, data.img_);

	stbi_image_free((void*)data.img_);

	updateMaterials();

	return true;
}

void GLSceneDataLazy::updateMaterials()
{
	const size_t numMaterials = materialsLoaded_.size();

	materials_.resize(numMaterials);

	for (size_t i = 0; i != numMaterials; i++)
	{
		const auto& in = materialsLoaded_[i];
		auto& out = materials_[i];
		out = in;
		out.ambientOcclusionMap_ = getTextureHandleBindless(in.ambientOcclusionMap_, allMaterialTextures_);
		out.emissiveMap_ = getTextureHandleBindless(in.emissiveMap_, allMaterialTextures_);
		out.albedoMap_ = getTextureHandleBindless(in.albedoMap_, allMaterialTextures_);
		out.metallicRoughnessMap_ = getTextureHandleBindless(in.metallicRoughnessMap_, allMaterialTextures_);
		out.normalMap_ = getTextureHandleBindless(in.normalMap_, allMaterialTextures_);
	}
}

void GLSceneDataLazy::loadScene(const char* sceneFile)
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
