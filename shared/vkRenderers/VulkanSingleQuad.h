#pragma once

#include "shared/vkRenderers/VulkanRendererBase.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

class VulkanSingleQuadRenderer: public RendererBase
{
public:
	VulkanSingleQuadRenderer(VulkanRenderDevice& vkDev, VulkanImage tex, VkSampler sampler, VkImageLayout desiredLayout = VK_IMAGE_LAYOUT_GENERAL);
	virtual ~VulkanSingleQuadRenderer();

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;
private:
	VulkanRenderDevice& vkDev;

	bool createDescriptorSet(VulkanRenderDevice& vkDev, VkImageLayout desiredLayout = VK_IMAGE_LAYOUT_GENERAL);

	VulkanImage texture;
	VkSampler textureSampler;
};
