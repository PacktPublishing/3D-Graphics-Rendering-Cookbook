#include "FinalRenderer.h"
#include <stb/stb_image.h>

BaseMultiRenderer::BaseMultiRenderer(
	VulkanRenderContext& ctx,
	VKSceneData& sceneData,
	const std::vector<int>& objectIndices,
	const char* vertShaderFile,
	const char* fragShaderFile,
	const std::vector<VulkanTexture>& outputs,
	RenderPass screenRenderPass,
	const std::vector<BufferAttachment>& auxBuffers,
	const std::vector<TextureAttachment>& auxTextures)
: Renderer(ctx)
, sceneData_(sceneData)
, indices_(objectIndices)
{
	const PipelineInfo pInfo = initRenderPass(PipelineInfo {}, outputs, screenRenderPass, ctx.screenRenderPass);

	const uint32_t indirectDataSize = (uint32_t)sceneData_.shapes_.size() * sizeof(VkDrawIndirectCommand);

	const size_t imgCount = ctx.vkDev.swapchainImages.size();
	uniforms_.resize(imgCount);
	shape_.resize(imgCount);
	indirect_.resize(imgCount);

	descriptorSets_.resize(imgCount);

	const uint32_t shapesSize = (uint32_t)sceneData_.shapes_.size() * sizeof(DrawData);
	const uint32_t uniformBufferSize = sizeof(ubo_);

	std::vector<TextureAttachment> textureAttachments;
	if (sceneData_.envMap_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.envMap_));
	if (sceneData_.envMapIrradiance_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.envMapIrradiance_));
	if (sceneData_.brdfLUT_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.brdfLUT_));

	for (const auto& t: auxTextures)
		textureAttachments.push_back(t);

	DescriptorSetInfo dsInfo = {
		.buffers = {
			uniformBufferAttachment(VulkanBuffer {},         0, uniformBufferSize, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			sceneData_.vertexBuffer_,
			sceneData_.indexBuffer_,
			storageBufferAttachment(VulkanBuffer {},         0, shapesSize, VK_SHADER_STAGE_VERTEX_BIT),
			storageBufferAttachment(sceneData_.material_,    0, (uint32_t)sceneData_.material_.size, VK_SHADER_STAGE_FRAGMENT_BIT),
			storageBufferAttachment(sceneData_.transforms_,  0, (uint32_t)sceneData_.transforms_.size, VK_SHADER_STAGE_VERTEX_BIT),
		},
		.textures = textureAttachments,
		.textureArrays = { sceneData_.allMaterialTextures }
	};

	for (const auto& b: auxBuffers)
		dsInfo.buffers.push_back(b);

	descriptorSetLayout_ = ctx.resources.addDescriptorSetLayout(dsInfo);
	descriptorPool_ = ctx.resources.addDescriptorPool(dsInfo, (uint32_t)imgCount);

	for (size_t i = 0; i != imgCount; i++)
	{
		uniforms_[i] = ctx.resources.addUniformBuffer(uniformBufferSize);
		indirect_[i] = ctx.resources.addIndirectBuffer(indirectDataSize);
		updateIndirectBuffers(i);

		shape_[i] = ctx.resources.addStorageBuffer(shapesSize);
		uploadBufferData(ctx.vkDev, shape_[i].memory, 0, sceneData_.shapes_.data(), shapesSize);

		dsInfo.buffers[0].buffer = uniforms_[i];
		dsInfo.buffers[3].buffer = shape_[i];

		descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
		ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
	}

	initPipeline({ vertShaderFile, fragShaderFile }, pInfo);
}

void BaseMultiRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb, VkRenderPass rp)
{
	beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);

	/* For CountKHR (Vulkan 1.1) we may use indirect rendering with GPU-based object counter */
	/// vkCmdDrawIndirectCountKHR(commandBuffer, indirectBuffers_[currentImage], 0, countBuffers_[currentImage], 0, shapes.size(), sizeof(VkDrawIndirectCommand));
	/* For Vulkan 1.0 vkCmdDrawIndirect is enough */
	vkCmdDrawIndirect(commandBuffer, indirect_[currentImage].buffer, 0, (uint32_t)sceneData_.shapes_.size(), sizeof(VkDrawIndirectCommand));

	vkCmdEndRenderPass(commandBuffer);
}

void BaseMultiRenderer::updateIndirectBuffers(size_t currentImage, bool* visibility)
{
	VkDrawIndirectCommand* data = nullptr;
	vkMapMemory(ctx_.vkDev.device, indirect_[currentImage].memory, 0, sizeof(VkDrawIndirectCommand), 0, (void**)&data);

	const uint32_t size = (uint32_t)indices_.size(); // (uint32_t)sceneData_.shapes_.size();

	for (uint32_t i = 0; i != size; i++)
	{
		const uint32_t j = sceneData_.shapes_[indices_[i]].meshIndex;

		const uint32_t lod = sceneData_.shapes_[indices_[i]].LOD;
		data[i] = {
			.vertexCount = sceneData_.meshData_.meshes_[j].getLODIndicesCount(lod),
			.instanceCount = visibility ? (visibility[indices_[i]] ? 1u : 0u) : 1u,
			.firstVertex = 0,
			.firstInstance = (uint32_t)indices_[i]
		};
	}
	vkUnmapMemory(ctx_.vkDev.device, indirect_[currentImage].memory);
}

bool FinalMultiRenderer::checkLoadedTextures()
{
	VKSceneData::LoadedImageData data;

	{
		std::lock_guard lock(sceneData_.loadedFilesMutex_);

		if (sceneData_.loadedFiles_.empty())
			return false;

		data = sceneData_.loadedFiles_.back();

		sceneData_.loadedFiles_.pop_back();
	}

	auto newTexture = ctx_.resources.addRGBATexture(data.w_, data.h_, const_cast<uint8_t*>(data.img_));

	transparentRenderer.updateTexture(data.index_, newTexture, 14);
	opaqueRenderer.updateTexture(data.index_, newTexture, 11);

	stbi_image_free((void*)data.img_);

	return true;
}
