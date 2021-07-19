#pragma once

#include "shared/vkRenderers/VulkanRendererBase.h"

class PBRModelRenderer: public RendererBase
{
public:
	PBRModelRenderer(VulkanRenderDevice& vkDev,
			uint32_t uniformBufferSize,
			const char* modelFile,
			const char* texAOFile,
			const char* texEmissiveFile,
			const char* texAlbedoFile,
			const char* texMeRFile,
			const char* texNormalFile,
			const char* texEnvMapFile,
			const char* texIrrMapFile,
			VulkanImage depthTexture);

	virtual ~PBRModelRenderer();

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

	void updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const void* data, const size_t dataSize);

private:
	size_t vertexBufferSize_;
	size_t indexBufferSize_;

	// 6. Storage Buffer with index and vertex data
	VkBuffer storageBuffer_;
	VkDeviceMemory storageBufferMemory_;

	VulkanTexture texAO_;
	VulkanTexture texEmissive_;
	VulkanTexture texAlbedo_;
	VulkanTexture texMeR_;
	VulkanTexture texNormal_;

	VulkanTexture envMapIrradiance_;
	VulkanTexture envMap_;

	VulkanTexture brdfLUT_;

	bool createDescriptorSet(VulkanRenderDevice& vkDev, uint32_t uniformDataSize);
};
