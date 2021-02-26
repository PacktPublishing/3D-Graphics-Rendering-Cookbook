#pragma once

#include "shared/vkFramework/Renderer.h"
#include "shared/scene/Scene.h"
#include "shared/scene/Material.h"
#include "shared/scene/VtxData.h"

// Container of mesh data, material data and scene nodes with transformations
struct VKSceneData
{
	VKSceneData(VulkanRenderContext& ctx,
		const char* meshFile,
		const char* sceneFile,
		const char* materialFile,
		VulkanTexture envMap,
		VulkanTexture irradienceMap);

	VulkanTexture envMapIrradience_;
	VulkanTexture envMap_;
	VulkanTexture brdfLUT_;

	VulkanBuffer material_;
	VulkanBuffer transforms_;

	VulkanRenderContext& ctx;

	TextureArrayAttachment allMaterialTextures;

	BufferAttachment indexBuffer_;
	BufferAttachment vertexBuffer_;

	Scene scene_;
	std::vector<MaterialDescription> materials_;
	std::vector<Mesh> meshes_;

	std::vector<glm::mat4> shapeTransforms_;

	std::vector<DrawData> shapes_;

	void loadScene(const char* sceneFile);
	void loadMeshes(const char* meshFile);

	void convertGlobalToShapeTransforms();
	void recalculateAllTransforms();
	void uploadGlobalTransforms();

	void updateMaterial(int matIdx);
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
		RenderPass screenRenderPass = RenderPass());

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override;
	void updateBuffers(size_t currentImage) override;

	void updateIndirectBuffers(size_t currentImage, bool* visibility = nullptr);

	inline void setMatrices(const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model) {
		proj_ = proj;
		view_ = view;
		model_ = model;
	}

	inline void setCameraPosition(const glm::vec3& cameraPos) {
		cameraPos_ = glm::vec4(cameraPos, 1.0f);
	}

	inline const VKSceneData& getSceneData() const { return sceneData_; }

private:
	VKSceneData& sceneData_;

	std::vector<VulkanBuffer> indirect_;
	std::vector<VulkanBuffer> shape_;

	glm::mat4 proj_, model_, view_;
	glm::vec4 cameraPos_;
};
