#pragma once

#include "shared/UtilsVulkanRendererBase.h"

class ModelRenderer: public RendererBase
{
public:
	ModelRenderer(VulkanRenderDevice& vkDev, const char* modelFile, const char* textureFile, uint32_t uniformDataSize);
	virtual ~ModelRenderer();

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

	void updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const void* data, const size_t dataSize);

private:
	size_t vertexBufferSize_;
	size_t indexBufferSize_;

	// 6. Storage Buffer with index and vertex data
	VkBuffer storageBuffer_;
	VkDeviceMemory storageBufferMemory_;

	VkSampler textureSampler_;
	VulkanImage texture_;

	bool createDescriptorSet(VulkanRenderDevice& vkDev, uint32_t uniformDataSize);
};
