#pragma once

#include "shared/vkRenderers/VulkanRendererBase.h"

class ModelRenderer: public RendererBase
{
public:
	ModelRenderer(VulkanRenderDevice& vkDev, const char* modelFile, const char* textureFile, uint32_t uniformDataSize);
	ModelRenderer(VulkanRenderDevice& vkDev, bool useDepth, VkBuffer storageBuffer, VkDeviceMemory storageBufferMemory, uint32_t vertexBufferSize, uint32_t indexBufferSize, VulkanImage texture, VkSampler textureSampler, const std::vector<const char*>& shaderFiles, uint32_t uniformDataSize, bool useGeneralTextureLayout = true, VulkanImage externalDepth = { .image = VK_NULL_HANDLE }, bool deleteMeshData = true);
	virtual ~ModelRenderer();

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

	void updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const void* data, const size_t dataSize);

	// HACK to allow sharing textures between multiple ModelRenderers
	void freeTextureSampler() { textureSampler_ = VK_NULL_HANDLE; }

private:
	bool useGeneralTextureLayout_ = false;
	bool isExternalDepth_ = false;
	bool deleteMeshData_ = true;

	size_t vertexBufferSize_;
	size_t indexBufferSize_;

	// 6. Storage Buffer with index and vertex data
	VkBuffer storageBuffer_;
	VkDeviceMemory storageBufferMemory_;

	VkSampler textureSampler_;
	VulkanImage texture_;

	bool createDescriptorSet(VulkanRenderDevice& vkDev, uint32_t uniformDataSize);
};
