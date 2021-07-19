#pragma once

#include "shared/vkRenderers/VulkanRendererBase.h"

class VulkanClear: public RendererBase
{
public:
	VulkanClear(VulkanRenderDevice& vkDev, VulkanImage depthTexture);

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

private:
	bool shouldClearDepth;
};
