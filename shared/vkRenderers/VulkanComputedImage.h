#pragma once

#include "shared/Utils.h"
#include "shared/vkRenderers/VulkanComputedItem.h"

struct ComputedImage: public ComputedItem
{
	ComputedImage(VulkanRenderDevice& vkDev, const char* shaderName, uint32_t textureWidth, uint32_t textureHeight, bool supportDownload = false);
	virtual ~ComputedImage() {}

	void downloadImage(void* imageData);

	VulkanImage computed;
	VkSampler   computedImageSampler;
	uint32_t computedWidth, computedHeight;

protected:
	bool canDownloadImage;

	bool createComputedTexture(uint32_t computedWidth, uint32_t computedHeight, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

	bool createDescriptorSet();
	bool createComputedImageSetLayout();
};
