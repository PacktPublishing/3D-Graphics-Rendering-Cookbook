#pragma once

#include "shared/vkFramework/Renderer.h"

/**
VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkImageLayout oldLayout VkImageLayout newLayout

// Before next stage (convert from )
ImageBarrier(ctx_, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
// Return back to attachment
ImageBarrier(ctx_, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL)
*/

struct ShaderOptimalToColorBarrier : public Renderer
{
	ShaderOptimalToColorBarrier(VulkanRenderContext& c, VulkanTexture tex):
		Renderer(c),
		tex_(tex)
	{}

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override
	{
		transitionImageLayoutCmd(cmdBuffer, tex_.image.image, tex_.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}

private:
	VulkanTexture tex_;
};

struct ShaderOptimalToDepthBarrier : public Renderer
{
	ShaderOptimalToDepthBarrier(VulkanRenderContext& c, VulkanTexture tex):
		Renderer(c),
		tex_(tex)
	{}

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override
	{
		transitionImageLayoutCmd(cmdBuffer, tex_.image.image, tex_.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

private:
	VulkanTexture tex_;
};

struct ColorToShaderOptimalBarrier : public Renderer
{
	ColorToShaderOptimalBarrier(VulkanRenderContext& c, VulkanTexture tex):
		Renderer(c),
		tex_(tex)
	{}

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override
	{
		transitionImageLayoutCmd(cmdBuffer, tex_.image.image, tex_.format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

private:
	VulkanTexture tex_;
};

struct ColorWaitBarrier : public Renderer
{
	ColorWaitBarrier(VulkanRenderContext& c, VulkanTexture tex):
		Renderer(c),
		tex_(tex)
	{}

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override
	{
		transitionImageLayoutCmd(cmdBuffer, tex_.image.image, tex_.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

private:
	VulkanTexture tex_;
};

struct DepthToShaderOptimalBarrier : public Renderer
{
	DepthToShaderOptimalBarrier(VulkanRenderContext& c, VulkanTexture tex):
		Renderer(c),
		tex_(tex)
	{}

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override
	{
		transitionImageLayoutCmd(cmdBuffer, tex_.image.image, tex_.format, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

private:
	VulkanTexture tex_;
};

struct ImageBarrier : public Renderer
{
	ImageBarrier(VulkanRenderContext& c, VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image):
		Renderer(c),
		srcAccess_(srcAccess),
		dstAccess_(dstAccess),
		oldLayout_(oldLayout),
		newLayout_(newLayout),
		image_(image)
	{}

	virtual void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE)
	{
		VkImageMemoryBarrier barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = srcAccess_,
			.dstAccessMask = dstAccess_,
			.oldLayout = oldLayout_,
			.newLayout = newLayout_,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = image_,
			.subresourceRange = VkImageSubresourceRange {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		vkCmdPipelineBarrier(
			cmdBuffer,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
#if 0
VkImageMemoryBarrier/*2KHR*/ imageMemoryBarrier = {
  .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
  .pNext = nullptr,
  .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
  .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
  .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
  .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT_KHR,
  .oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
  .newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
  /* .image and .subresourceRange should identify image subresource accessed */};

vkCmdPipelineBarrier2KHR(
    ...
    1,                      // imageMemoryBarrierCount
    &imageMemoryBarrier,    // pImageMemoryBarriers
    ...);

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 1, &readoutBarrier, 0, nullptr, 0, nullptr);
#endif
	}
private:
	VkAccessFlags srcAccess_;
	VkAccessFlags dstAccess_;
	VkImageLayout oldLayout_;
	VkImageLayout newLayout_;
	VkImage image_;
};
