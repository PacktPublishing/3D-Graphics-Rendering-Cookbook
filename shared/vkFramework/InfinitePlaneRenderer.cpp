#include "shared/vkFramework/InfinitePlaneRenderer.h"

void InfinitePlaneRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb, VkRenderPass rp)
{
	beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, 6, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
}

void InfinitePlaneRenderer::updateBuffers(size_t currentImage)
{
	const UniformBuffer ubo = { proj_, view_, model_, (float)glfwGetTime() };
	uploadBufferData(ctx_.vkDev, uniforms_[currentImage].memory, 0, &ubo, sizeof(ubo));
}

InfinitePlaneRenderer::InfinitePlaneRenderer(VulkanRenderContext& ctx,
	const std::vector<VulkanTexture>& outputs,
	RenderPass screenRenderPass):
	Renderer(ctx)
{
	const PipelineInfo pInfo = initRenderPass(PipelineInfo{}, outputs, screenRenderPass, ctx.screenRenderPass_NoDepth);

	const size_t imgCount = ctx.vkDev.swapchainImages.size();
	descriptorSets_.resize(imgCount);
	uniforms_.resize(imgCount);

	DescriptorSetInfo dsInfo = {
		.buffers = {
			uniformBufferAttachment(VulkanBuffer {}, 0, sizeof(UniformBuffer), VK_SHADER_STAGE_VERTEX_BIT)
		}
	};

	descriptorSetLayout_ = ctx_.resources.addDescriptorSetLayout(dsInfo);
	descriptorPool_ = ctx_.resources.addDescriptorPool(dsInfo, imgCount);

	for (size_t i = 0 ; i != imgCount ; i++)
	{
		uniforms_[i] = ctx.resources.addUniformBuffer(sizeof(UniformBuffer));
		dsInfo.buffers[0].buffer = uniforms_[i];

		descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
		ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
	}

	initPipeline({ "data/shaders/chapter07/VK03_InfinitePlane.vert", "data/shaders/chapter07/VK03_InfinitePlane.frag" }, pInfo);
}
