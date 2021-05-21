#pragma once

#include "shared/scene/Scene.h"
#include "shared/scene/Material.h"
#include "shared/scene/VtxData.h"
#include "shared/glFramework/GLShader.h"
#include "shared/glFramework/GLTexture.h"

class GLSceneData
{
public:
	GLSceneData(
		const char* meshFile,
		const char* sceneFile,
		const char* materialFile);

	std::vector<GLTexture> allMaterialTextures_;

	MeshFileHeader header_;
	MeshData meshData_;

	Scene scene_;
	std::vector<MaterialDescription> materials_;
	std::vector<DrawData> shapes_;

	void loadScene(const char* sceneFile);
};
