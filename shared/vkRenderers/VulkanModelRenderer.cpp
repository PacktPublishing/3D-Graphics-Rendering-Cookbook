#include "shared/vkRenderers/VulkanModelRenderer.h"
#include "shared/EasyProfilerWrapper.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;

static constexpr VkClearColorValue clearValueColor = { 1.0f, 1.0f, 1.0f, 1.0f };

bool ModelRenderer::createDescriptorSet(VulkanRenderDevice& vkDev, uint32_t uniformDataSize)
{
	const std::array<VkDescriptorSetLayoutBinding, 4> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	const VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device, &layoutInfo, nullptr, &descriptorSetLayout_));

	std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(), descriptorSetLayout_);

	const VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool_,
		.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),
		.pSetLayouts = layouts.data()
	};

	descriptorSets_.resize(vkDev.swapchainImages.size());

	VK_CHECK(vkAllocateDescriptorSets(vkDev.device, &allocInfo, descriptorSets_.data()));

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		VkDescriptorSet ds = descriptorSets_[i];

		const VkDescriptorBufferInfo bufferInfo  = { uniformBuffers_[i], 0, uniformDataSize };
		const VkDescriptorBufferInfo bufferInfo2 = { storageBuffer_, 0, vertexBufferSize_ };
		const VkDescriptorBufferInfo bufferInfo3 = { storageBuffer_, vertexBufferSize_, indexBufferSize_ };
		const VkDescriptorImageInfo  imageInfo   = { textureSampler_, texture_.imageView, useGeneralTextureLayout_ ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		const std::array<VkWriteDescriptorSet, 4> descriptorWrites = {
			bufferWriteDescriptorSet(ds, &bufferInfo,  0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo2, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo3, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
			imageWriteDescriptorSet( ds, &imageInfo,   3)
		};

		vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return true;
}

void ModelRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	EASY_FUNCTION();

	beginRenderPass(commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(indexBufferSize_ / (sizeof(unsigned int))), 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
}

void ModelRenderer::updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const void* data, const size_t dataSize)
{
	uploadBufferData(vkDev, uniformBuffersMemory_[currentImage], 0, data, dataSize);
}

ModelRenderer::ModelRenderer(VulkanRenderDevice& vkDev, const char* modelFile, const char* textureFile, uint32_t uniformDataSize)
: RendererBase(vkDev, VulkanImage())
{
	// Resource loading part
	if (!createTexturedVertexBuffer(vkDev, modelFile, &storageBuffer_, &storageBufferMemory_, &vertexBufferSize_, &indexBufferSize_))
	{
		printf("ModelRenderer: createTexturedVertexBuffer() failed\n");
		exit(EXIT_FAILURE);
	}

	createTextureImage(vkDev, textureFile, texture_.image, texture_.imageMemory);
	createImageView(vkDev.device, texture_.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &texture_.imageView);
	createTextureSampler(vkDev.device, &textureSampler_);

	if (!createDepthResources(vkDev, vkDev.framebufferWidth, vkDev.framebufferHeight, depthTexture_) ||
		!createColorAndDepthRenderPass(vkDev, true, &renderPass_, RenderPassCreateInfo()) ||
		!createUniformBuffers(vkDev, uniformDataSize) ||
		!createColorAndDepthFramebuffers(vkDev, renderPass_, depthTexture_.imageView, swapchainFramebuffers_) ||
		!createDescriptorPool(vkDev, 1, 2, 1, &descriptorPool_) ||
		!createDescriptorSet(vkDev, uniformDataSize) ||
		!createPipelineLayout(vkDev.device, descriptorSetLayout_, &pipelineLayout_) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_, {"data/shaders/chapter03/VK02.vert", "data/shaders/chapter03/VK02.frag", "data/shaders/chapter03/VK02.geom" }, &graphicsPipeline_))
	{
		printf("ModelRenderer: failed to create pipeline\n");
		exit(EXIT_FAILURE);
	}
}

ModelRenderer::ModelRenderer(VulkanRenderDevice& vkDev, bool useDepth, VkBuffer storageBuffer, VkDeviceMemory storageBufferMemory, uint32_t vertexBufferSize, uint32_t indexBufferSize, VulkanImage texture, VkSampler textureSampler, const std::vector<const char*>& shaderFiles, uint32_t uniformDataSize, bool useGeneralTextureLayout, VulkanImage externalDepth, bool deleteMeshData)
	: useGeneralTextureLayout_(useGeneralTextureLayout)
	, vertexBufferSize_(vertexBufferSize)
	, indexBufferSize_(indexBufferSize)
	, storageBuffer_(storageBuffer)
	, storageBufferMemory_(storageBufferMemory)
	, texture_(texture)
	, textureSampler_(textureSampler)
	, deleteMeshData_(deleteMeshData)
	, RendererBase(vkDev, VulkanImage())
{
	if (useDepth)
	{
		isExternalDepth_ = (externalDepth.image != VK_NULL_HANDLE);

		if (isExternalDepth_)
			depthTexture_ = externalDepth;
		else
			createDepthResources(vkDev, vkDev.framebufferWidth, vkDev.framebufferHeight, depthTexture_);
	}

	if (	!createColorAndDepthRenderPass(vkDev, useDepth, &renderPass_, RenderPassCreateInfo()) ||
		!createUniformBuffers(vkDev, uniformDataSize) ||
		!createColorAndDepthFramebuffers(vkDev, renderPass_, depthTexture_.imageView, swapchainFramebuffers_) ||
		!createDescriptorPool(vkDev, 1, 2, 1, &descriptorPool_) ||
		!createDescriptorSet(vkDev, uniformDataSize) ||
		!createPipelineLayout(vkDev.device, descriptorSetLayout_, &pipelineLayout_) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_, shaderFiles, &graphicsPipeline_))
	{                                                                                               
		printf("ModelRenderer: failed to create pipeline\n");
		exit(EXIT_FAILURE);
	}
}

ModelRenderer::~ModelRenderer()
{
	if (deleteMeshData_)
	{
		vkDestroyBuffer(device_, storageBuffer_, nullptr);
		vkFreeMemory(device_, storageBufferMemory_, nullptr);
	}

	if (textureSampler_ != VK_NULL_HANDLE)
	{
		vkDestroySampler(device_, textureSampler_, nullptr);
		destroyVulkanImage(device_, texture_);
	}

	if (!isExternalDepth_)
		destroyVulkanImage(device_, depthTexture_);
}
