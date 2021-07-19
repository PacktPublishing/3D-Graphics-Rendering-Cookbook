#include "shared/Utils.h"
#include "shared/UtilsMath.h"
#include "shared/vkRenderers/VulkanFinish.h"
#include "shared/EasyProfilerWrapper.h"

VulkanFinish::VulkanFinish(VulkanRenderDevice& vkDev, VulkanImage depthTexture)
: RendererBase(vkDev, depthTexture)
{
	if (!createColorAndDepthRenderPass(
		vkDev, (depthTexture.image != VK_NULL_HANDLE), &renderPass_, RenderPassCreateInfo{ .clearColor_ = false, .clearDepth_ = false, .flags_ = eRenderPassBit_Last }))
	{
		printf("VulkanFinish: failed to create render pass\n");
		exit(EXIT_FAILURE);
	}

	createColorAndDepthFramebuffers(vkDev, renderPass_, depthTexture.imageView, swapchainFramebuffers_);
}

void VulkanFinish::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	EASY_FUNCTION();

	const VkRect2D screenRect = {
		.offset = { 0, 0 },
		.extent = {.width = framebufferWidth_, .height = framebufferHeight_ }
	};

	const VkRenderPassBeginInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass_,
		.framebuffer = swapchainFramebuffers_[currentImage],
		.renderArea = screenRect
	};

	vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
	vkCmdEndRenderPass( commandBuffer );
}
