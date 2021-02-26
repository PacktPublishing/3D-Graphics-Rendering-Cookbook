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

	std::vector<uint32_t> indexData_;
	std::vector<float> vertexData_;
	MeshFileHeader header_;

	Scene scene_;
	std::vector<MaterialDescription> materials_;
	std::vector<Mesh> meshes_;

	std::vector<DrawData> shapes_;

	void loadScene(const char* sceneFile);
};
