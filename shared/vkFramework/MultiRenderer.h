#pragma once

#include "shared/vkFramework/Renderer.h"
#include "shared/scene/Scene.h"
#include "shared/scene/Material.h"
#include "shared/scene/VtxData.h"

#include <taskflow/taskflow.hpp>

// Container of mesh data, material data and scene nodes with transformations
struct VKSceneData
{
	VKSceneData(VulkanRenderContext& ctx,
		const char* meshFile,
		const char* sceneFile,
		const char* materialFile,
		VulkanTexture envMap,
		VulkanTexture irradianceMap,
		bool asyncLoad = false);

	VulkanTexture envMapIrradiance_;
	VulkanTexture envMap_;
	VulkanTexture brdfLUT_;

	VulkanBuffer material_;
	VulkanBuffer transforms_;

	VulkanRenderContext& ctx;

	TextureArrayAttachment allMaterialTextures;

	BufferAttachment indexBuffer_;
	BufferAttachment vertexBuffer_;

	MeshData meshData_;

	Scene scene_;
	std::vector<MaterialDescription> materials_;

	std::vector<glm::mat4> shapeTransforms_;

	std::vector<DrawData> shapes_;

	void loadScene(const char* sceneFile);
	void loadMeshes(const char* meshFile);

	void convertGlobalToShapeTransforms();
	void recalculateAllTransforms();
	void uploadGlobalTransforms();

	void updateMaterial(int matIdx);

	/* Chapter 9, async loading */
	struct LoadedImageData
	{
		int index_ = 0;
		int w_ = 0;
		int h_ = 0;
		const uint8_t* img_ = nullptr;
	};

	std::vector<std::string> textureFiles_;
	std::vector<LoadedImageData> loadedFiles_;
	std::mutex loadedFilesMutex_;

private:
	tf::Taskflow taskflow_;
	tf::Executor executor_;
};

constexpr const char* DefaultMeshVertexShader = "data/shaders/chapter07/VK01.vert";
constexpr const char* DefaultMeshFragmentShader = "data/shaders/chapter07/VK01.frag";

struct MultiRenderer: public Renderer
{
	MultiRenderer(
		VulkanRenderContext& ctx,
		VKSceneData& sceneData,
		const char* vtxShaderFile = DefaultMeshVertexShader,
		const char* fragShaderFile = DefaultMeshFragmentShader,
		const std::vector<VulkanTexture>& outputs = std::vector<VulkanTexture> {},
		RenderPass screenRenderPass = RenderPass(),
		const std::vector<BufferAttachment>& auxBuffers = std::vector<BufferAttachment> {},
		const std::vector<TextureAttachment>& auxTextures = std::vector<TextureAttachment> {});

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override;
	void updateBuffers(size_t currentImage) override;

	void updateIndirectBuffers(size_t currentImage, bool* visibility = nullptr);

	inline void setMatrices(const glm::mat4& proj, const glm::mat4& view) {
		const glm::mat4 m1 = glm::scale(glm::mat4(1.f), glm::vec3(1.f, -1.f, 1.f));
		ubo_.proj_ = proj;
		ubo_.view_ = view * m1;
	}

	inline void setCameraPosition(const glm::vec3& cameraPos) {
		ubo_.cameraPos_ = glm::vec4(cameraPos, 1.0f);
	}

	inline const VKSceneData& getSceneData() const { return sceneData_; }

	// Async loading in Chapter9
	bool checkLoadedTextures();

private:
	VKSceneData& sceneData_;

	std::vector<VulkanBuffer> indirect_;
	std::vector<VulkanBuffer> shape_;

	struct UBO {
		mat4 proj_;
		mat4 view_;
		vec4 cameraPos_;
	} ubo_;
};
