#include "shared/vkFramework/CubeRenderer.h"

void CubemapRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb, VkRenderPass rp)
{
	beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, 36, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
}

CubemapRenderer::CubemapRenderer(VulkanRenderContext& ctx,
	VulkanTexture texture,
	const std::vector<VulkanTexture>& outputs,
	RenderPass screenRenderPass):
	Renderer(ctx)
{
	const PipelineInfo pInfo = initRenderPass(PipelineInfo{}, outputs, screenRenderPass, ctx.screenRenderPass);

	const size_t imgCount = ctx.vkDev.swapchainImages.size();
	descriptorSets_.resize(imgCount);
	uniforms_.resize(imgCount);

	DescriptorSetInfo dsInfo = {
		.buffers = { uniformBufferAttachment(VulkanBuffer {}, 0, sizeof(UniformBuffer), VK_SHADER_STAGE_VERTEX_BIT) },
		.textures = { fsTextureAttachment(texture) }
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

	initPipeline({ "data/shaders/chapter08/VK03_CubeMap.vert", "data/shaders/chapter08/VK03_CubeMap.frag" }, pInfo);
}

void CubemapRenderer::updateBuffers(size_t currentImage)
{
	updateUniformBuffer(currentImage, 0, sizeof(UniformBuffer), &ubo);
}
