#pragma once

#include "shared/vkFramework/Renderer.h"

/* 
   @brief Shader (post)processor for fullscreen effects

   Multiple input textures, single output [color + depth]. Possibly, can be extented to multiple outputs (allocate appropriate framebuffer)
*/
struct VulkanShaderProcessor: public Renderer
{
	VulkanShaderProcessor(VulkanRenderContext& ctx,
		const PipelineInfo& pInfo,
		const DescriptorSetInfo& dsInfo,
		const std::vector<const char*>& shaders,
		const std::vector<VulkanTexture>& outputs,
		uint32_t indexBufferSize = 6 * 4,
		RenderPass screenRenderPass = RenderPass());

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override;

private:
	uint32_t indexBufferSize;
};

struct QuadProcessor: public VulkanShaderProcessor
{
	QuadProcessor(VulkanRenderContext& ctx, const DescriptorSetInfo& dsInfo,
		const std::vector<VulkanTexture>& outputs, const char* shaderFile):
		VulkanShaderProcessor(ctx, ctx.pipelineParametersForOutputs(outputs),  dsInfo,
			std::vector<const char*> { "data/shaders/chapter08/VK02_Quad.vert", shaderFile },
			outputs, 6 * 4, outputs.empty() ? ctx.screenRenderPass : RenderPass())
	{}
};

struct BufferProcessor: public VulkanShaderProcessor
{
	BufferProcessor(VulkanRenderContext& ctx, const DescriptorSetInfo& dsInfo,
		const std::vector<VulkanTexture>& outputs, const std::vector<const char*>& shaderFiles,
		uint32_t indexBufferSize = 6 * 4, RenderPass renderPass = RenderPass()):
		VulkanShaderProcessor(ctx, ctx.pipelineParametersForOutputs(outputs),  dsInfo,
		shaderFiles, outputs, indexBufferSize, outputs.empty() ? ctx.screenRenderPass : renderPass)
	{}
};

/* Commonly used BufferProcessor for single mesh rendering */
struct OffscreenMeshRenderer: public BufferProcessor
{
	OffscreenMeshRenderer(
		VulkanRenderContext& ctx,
		VulkanBuffer uniformBuffer,
		const std::pair<BufferAttachment, BufferAttachment>& meshBuffer,
		const std::vector<TextureAttachment>& usedTextures,
		const std::vector<VulkanTexture>& outputs,
		const std::vector<const char*>& shaderFiles,
		bool firstPass = false):

		BufferProcessor(ctx,
			DescriptorSetInfo {
				.buffers = {
					uniformBufferAttachment(uniformBuffer,  0, 0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
					meshBuffer.first,
					meshBuffer.second,
				},
				.textures = usedTextures
			},
			outputs, shaderFiles, meshBuffer.first.size,
			ctx.resources.addRenderPass(outputs, RenderPassCreateInfo {
			.clearColor_ = firstPass, .clearDepth_ = firstPass, .flags_ = (uint8_t)((firstPass ? eRenderPassBit_First : eRenderPassBit_OffscreenInternal) | eRenderPassBit_Offscreen) }))
	{
	}
};
