#include "shared/Utils.h"
#include "shared/UtilsMath.h"
#include "shared/vkRenderers/VulkanClear.h"
#include "shared/EasyProfilerWrapper.h"

VulkanClear::VulkanClear(VulkanRenderDevice& vkDev, VulkanImage depthTexture)
: RendererBase(vkDev, depthTexture)
, shouldClearDepth(depthTexture.image != VK_NULL_HANDLE)
{
	if (!createColorAndDepthRenderPass(
		vkDev, shouldClearDepth, &renderPass_, RenderPassCreateInfo{ .clearColor_ = true, .clearDepth_ = true, .flags_ = eRenderPassBit_First }))
	{
		printf("VulkanClear: failed to create render pass\n");
		exit(EXIT_FAILURE);
	}

	createColorAndDepthFramebuffers(vkDev, renderPass_, depthTexture.imageView, swapchainFramebuffers_);
}

void VulkanClear::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t swapFramebuffer)
{
	EASY_FUNCTION();

	const VkClearValue clearValues[2] =
	{
		VkClearValue { .color = { 1.0f, 1.0f, 1.0f, 1.0f } },
		VkClearValue { .depthStencil = { 1.0f, 0 } }
	};

	const VkRect2D screenRect = {
		.offset = { 0, 0 },
		.extent = {.width = framebufferWidth_, .height = framebufferHeight_ }
	};

	const VkRenderPassBeginInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass_,
		.framebuffer = swapchainFramebuffers_[swapFramebuffer],
		.renderArea = screenRect,
		.clearValueCount = static_cast<uint32_t>(shouldClearDepth ? 2 : 1),
		.pClearValues = &clearValues[0]
	};

	vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
	vkCmdEndRenderPass( commandBuffer );
}
