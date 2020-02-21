#pragma once

#include "shared/UtilsVulkanRendererBase.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;

class MultiRenderer: public RendererBase
{
public:
	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

	bool init(
		VulkanRenderDevice& vkDev,
		int maxVtxSize, int maxIdxSize,
		int numInstances,
		int instanceDataSize, int materialDataSize,
		const char* vtxShaderFile,
		const char* fragShaderFile);

	void updateIndirectBuffers(VulkanRenderDevice& vkDev, size_t currentImage);

	void updateGeometryBuffers(VulkanRenderDevice& vkDev, int vertexCount, int indexCount, const void* vertices, const void* indices);
	void updateMaterialBuffer(VulkanRenderDevice& vkDev, int materialSize, const void* materialData);

	void updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const mat4& m);
	void updateInstanceBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, int instanceSize, const void* instanceData);
	void updateCountBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, uint32_t itemCount);

	void destroy(VkDevice device);

	size_t vertexBufferSize_;
	size_t indexBufferSize_;

private:
	size_t maxVertexBufferSize_;
	size_t maxIndexBufferSize_;

	size_t maxInstances_;

	size_t maxInstanceSize_;
	size_t maxMaterialSize_;

	// 6. Storage Buffer with index and vertex data
	VkBuffer storageBuffer_;
	VkDeviceMemory storageBufferMemory_;

	VkBuffer materialBuffer_;
	VkDeviceMemory materialBufferMemory_;

	std::vector<VkBuffer> indirectBuffers_;
	std::vector<VkDeviceMemory> indirectBuffersMemory_;

	std::vector<VkBuffer> instanceBuffers_;
	std::vector<VkDeviceMemory> instanceBuffersMemory_;

	// Buffer for draw count
	std::vector<VkBuffer> countBuffers_;
	std::vector<VkDeviceMemory> countBuffersMemory_;

	bool createDescriptorSet(VulkanRenderDevice& vkDev);
};
