#pragma once

#include "shared/vkRenderers/VulkanRendererBase.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;

#include "shared/scene/VtxData.h"

class MultiMeshRenderer: public RendererBase
{
public:
	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

	MultiMeshRenderer(
		VulkanRenderDevice& vkDev,
		const char* meshFile,
		const char* drawDataFile,
		const char* materialFile,
		const char* vtxShaderFile,
		const char* fragShaderFile);

	void updateIndirectBuffers(VulkanRenderDevice& vkDev, size_t currentImage, bool* visibility = nullptr);

	void updateGeometryBuffers(VulkanRenderDevice& vkDev, uint32_t vertexCount, uint32_t indexCount, const void* vertices, const void* indices);
	void updateMaterialBuffer(VulkanRenderDevice& vkDev, uint32_t materialSize, const void* materialData);

	void updateUniformBuffer(VulkanRenderDevice& vkDev, size_t currentImage, const mat4& m);
	void updateDrawDataBuffer(VulkanRenderDevice& vkDev, size_t currentImage, uint32_t drawDataSize, const void* drawData);
	void updateCountBuffer(VulkanRenderDevice& vkDev, size_t currentImage, uint32_t itemCount);

	virtual ~MultiMeshRenderer();

	uint32_t vertexBufferSize_;
	uint32_t indexBufferSize_;

private:
	VulkanRenderDevice& vkDev;

	uint32_t maxVertexBufferSize_;
	uint32_t maxIndexBufferSize_;

	uint32_t maxShapes_;

	uint32_t maxDrawDataSize_;
	uint32_t maxMaterialSize_;

	// 6. Storage Buffer with index and vertex data
	VkBuffer storageBuffer_;
	VkDeviceMemory storageBufferMemory_;

	VkBuffer materialBuffer_;
	VkDeviceMemory materialBufferMemory_;

	std::vector<VkBuffer> indirectBuffers_;
	std::vector<VkDeviceMemory> indirectBuffersMemory_;

	std::vector<VkBuffer> drawDataBuffers_;
	std::vector<VkDeviceMemory> drawDataBuffersMemory_;

	// Buffer for draw count
	std::vector<VkBuffer> countBuffers_;
	std::vector<VkDeviceMemory> countBuffersMemory_;

	/* DrawData loaded from file. Converted to indirectBuffers[] and uploaded to drawDataBuffers[] */
	std::vector<DrawData> shapes;
	MeshData meshData_;

	bool createDescriptorSet(VulkanRenderDevice& vkDev);

	void loadDrawData(const char* drawDataFile);
};
