#pragma once

#include "VulkanApp.h"

struct Renderer
{
	Renderer(VulkanRenderContext& c)
	: processingWidth(c.vkDev.framebufferWidth)
	, processingHeight(c.vkDev.framebufferHeight)
	, ctx_(c)
	{}

	virtual void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) = 0;
	virtual void updateBuffers(size_t currentImage) {}

	inline void updateUniformBuffer(uint32_t currentImage, const uint32_t offset, const uint32_t size, const void* data) {
		uploadBufferData(ctx_.vkDev, uniforms_[currentImage].memory, offset, data, size);
	}

	void initPipeline(const std::vector<const char*>& shaders, const PipelineInfo& pInfo, uint32_t vtxConstSize = 0, uint32_t fragConstSize = 0)
	{
		pipelineLayout_ = ctx_.resources.addPipelineLayout(descriptorSetLayout_, vtxConstSize, fragConstSize);
		graphicsPipeline_ = ctx_.resources.addPipeline(renderPass_.handle, pipelineLayout_, shaders, pInfo);
	}

	PipelineInfo initRenderPass(const PipelineInfo& pInfo, const std::vector<VulkanTexture>& outputs,
		RenderPass renderPass = RenderPass(),
		RenderPass fallbackPass = RenderPass())
	{
		PipelineInfo outInfo = pInfo;
		if (!outputs.empty()) // offscreen rendering
		{
			printf("Creating framebuffer (outputs = %d). Output0: %dx%d; Output1: %dx%d\n", 
				(int)outputs.size(), outputs[0].width, outputs[0].height,
				(outputs.size() > 1 ? outputs[1].width : 0), (outputs.size() > 1 ? outputs[1].height : 0));
			fflush(stdout);

			processingWidth = outputs[0].width;
			processingHeight = outputs[0].height;

			outInfo.width = processingWidth;
			outInfo.height = processingHeight;

			renderPass_  = (renderPass.handle != VK_NULL_HANDLE) ? renderPass :
					((isDepthFormat(outputs[0].format) && (outputs.size() == 1)) ? ctx_.resources.addDepthRenderPass(outputs) : ctx_.resources.addRenderPass(outputs, RenderPassCreateInfo(), true));
			framebuffer_ = ctx_.resources.addFramebuffer(renderPass_, outputs);
		} else
		{
			renderPass_ = (renderPass.handle != VK_NULL_HANDLE) ? renderPass : fallbackPass;
		}
		return outInfo;
	}

	void beginRenderPass(VkRenderPass rp, VkFramebuffer fb, VkCommandBuffer commandBuffer, size_t currentImage)
	{
		const VkClearValue clearValues[2] = {
			VkClearValue { .color = { 1.0f, 1.0f, 1.0f, 1.0f } },
			VkClearValue { .depthStencil = { 1.0f, 0 } }
		};

		const VkRect2D rect {
			.offset = { 0, 0 },
			.extent = { .width = processingWidth, .height = processingHeight }
		};

		ctx_.beginRenderPass(commandBuffer, rp, currentImage, rect,
			fb,
			(renderPass_.info.clearColor_ ? 1u : 0u) + (renderPass_.info.clearDepth_ ? 1u : 0u),
			renderPass_.info.clearColor_ ? &clearValues[0] : (renderPass_.info.clearDepth_ ? &clearValues[1] : nullptr));

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &descriptorSets_[currentImage], 0, nullptr);
	}

	VkFramebuffer framebuffer_ = nullptr;
	RenderPass renderPass_;

	uint32_t processingWidth;
	uint32_t processingHeight;

	// Updating individual textures (9 is the binding in our Chapter7-Chapter9 IBL scene shaders)
	void updateTexture(uint32_t textureIndex, VulkanTexture newTexture, uint32_t bindingIndex = 9)
	{
		for (auto ds: descriptorSets_)
			updateTextureInDescriptorSetArray(ctx_.vkDev, ds, newTexture, textureIndex, bindingIndex);
	}

protected:
	VulkanRenderContext& ctx_;

	// Descriptor set (layout + pool + sets) -> uses uniform buffers, textures, framebuffers
	VkDescriptorSetLayout descriptorSetLayout_ = nullptr;
	VkDescriptorPool descriptorPool_ = nullptr;
	std::vector<VkDescriptorSet> descriptorSets_;

	// 4. Pipeline & render pass (using DescriptorSets & pipeline state options)
	VkPipelineLayout pipelineLayout_ = nullptr;
	VkPipeline graphicsPipeline_ = nullptr;

	std::vector<VulkanBuffer> uniforms_;
};
