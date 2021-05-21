#include "shared/vkFramework/QuadRenderer.h"

static constexpr int MAX_QUADS = 256;

void QuadRenderer::quad(float x1, float y1, float x2, float y2, int texIdx)
{
	VertexData v1 { { x1, y1, 0 }, { 0, 0 }, texIdx };
	VertexData v2 { { x2, y1, 0 }, { 1, 0 }, texIdx };
	VertexData v3 { { x2, y2, 0 }, { 1, 1 }, texIdx };
	VertexData v4 { { x1, y2, 0 }, { 0, 1 }, texIdx };

	quads_.push_back( v1 ); quads_.push_back( v2 ); quads_.push_back( v3 );
	quads_.push_back( v1 ); quads_.push_back( v3 ); quads_.push_back( v4 );
}

void QuadRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb, VkRenderPass rp)
{
	if (quads_.empty())
		return;

	beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(quads_.size()), 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
}

void QuadRenderer::updateBuffers(size_t currentImage)
{
	if (!quads_.empty())
		uploadBufferData(ctx_.vkDev, storages_[currentImage].memory, 0, quads_.data(), quads_.size() * sizeof(VertexData));
}

QuadRenderer::QuadRenderer(VulkanRenderContext& ctx,
	const std::vector<VulkanTexture>& textures,
	const std::vector<VulkanTexture>& outputs,
	RenderPass screenRenderPass):
	Renderer(ctx)
{
	const PipelineInfo pInfo = initRenderPass(PipelineInfo {}, outputs, screenRenderPass, ctx.screenRenderPass_NoDepth);

	uint32_t vertexBufferSize = MAX_QUADS * 6 * sizeof(VertexData);

	const size_t imgCount = ctx.vkDev.swapchainImages.size();
	descriptorSets_.resize(imgCount);
	storages_.resize(imgCount);

	DescriptorSetInfo dsInfo = {
		.buffers = {
			storageBufferAttachment(VulkanBuffer {}, 0, vertexBufferSize,     VK_SHADER_STAGE_VERTEX_BIT)
		},
		.textureArrays = { fsTextureArrayAttachment(textures) }
	};

	descriptorSetLayout_ = ctx.resources.addDescriptorSetLayout(dsInfo);
	descriptorPool_ = ctx.resources.addDescriptorPool(dsInfo, imgCount);

	for (size_t i = 0 ; i < imgCount ; i++)
	{
		storages_[i] = ctx.resources.addStorageBuffer(vertexBufferSize);
		dsInfo.buffers[0].buffer = storages_[i];
		descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
		ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
	}

	initPipeline({ "data/shaders/chapter08/VK02_QuadRenderer.vert", "data/shaders/chapter08/VK02_QuadRenderer.frag" }, pInfo);
}
