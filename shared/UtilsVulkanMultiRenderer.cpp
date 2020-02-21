#include "UtilsVulkanMultiRenderer.h"

bool MultiRenderer::createDescriptorSet(VulkanRenderDevice& vkDev)
{
	const std::array<VkDescriptorSetLayoutBinding, 5> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		/* vertices [part of this.storageBuffer] */
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		/* indices [part of this.storageBuffer] */
		descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		/* instance data [this.instanceBuffer] */
		descriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		/* material data [this.materialBuffer] */
		descriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
		/* dynamic array of textures (indexed by materialData[instance].mapIndex etc.) */
//		descriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
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

		const VkDescriptorBufferInfo bufferInfo  = { uniformBuffers_[i], 0, sizeof(mat4) };
		const VkDescriptorBufferInfo bufferInfo2 = { storageBuffer_, 0, maxVertexBufferSize_ };
		const VkDescriptorBufferInfo bufferInfo3 = { storageBuffer_, maxVertexBufferSize_, maxIndexBufferSize_ };
		const VkDescriptorBufferInfo bufferInfo4 = { instanceBuffers_[i], 0, maxInstanceSize_ };
////		const VkDescriptorBufferInfo bufferInfo4 = { indirectBuffers[i], 0, sizeof(VkDrawIndirectCommand/*maxInstanceSize*/) };
		const VkDescriptorBufferInfo bufferInfo5 = { materialBuffer_, 0, maxMaterialSize_ };
//		const VkDescriptorImageInfo  imageInfo   = { textureSampler, texture.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		const std::array<VkWriteDescriptorSet, 5> descriptorWrites = {
			bufferWriteDescriptorSet(ds, &bufferInfo,  0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo2, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo3, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo4, 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo5, 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
//			imageWriteDescriptorSet( ds, &imageInfo,   3)
		};

		vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return true;
}

void MultiRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	beginRenderPass(commandBuffer, currentImage);
	vkCmdDrawIndirectCountKHR(commandBuffer, indirectBuffers_[currentImage], 0, countBuffers_[currentImage], 0, 128, sizeof(VkDrawIndirectCommand));
	vkCmdEndRenderPass(commandBuffer);
}

void MultiRenderer::updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const mat4& m)
{
	uploadBufferData(vkDev, uniformBuffersMemory_[currentImage], 0, glm::value_ptr(m), sizeof(mat4));
}

void MultiRenderer::updateGeometryBuffers(VulkanRenderDevice& vkDev, int vertexCount, int indexCount, const void* vertices, const void* indices)
{
	uploadBufferData(vkDev, storageBufferMemory_, 0, vertices, vertexCount);
	uploadBufferData(vkDev, storageBufferMemory_, maxVertexBufferSize_, indices, indexCount);
}

void MultiRenderer::updateInstanceBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, int instanceSize, const void* instanceData)
{
}

void MultiRenderer::updateMaterialBuffer(VulkanRenderDevice& vkDev, int materialSize, const void* materialData)
{
}

void MultiRenderer::updateCountBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, uint32_t itemCount)
{
	uploadBufferData(vkDev, countBuffersMemory_[currentImage], 0, &itemCount, sizeof(uint32_t));
}

void MultiRenderer::updateIndirectBuffers(VulkanRenderDevice& vkDev, size_t currentImage)
{
	VkDrawIndirectCommand* data = nullptr;
	vkMapMemory(vkDev.device, indirectBuffersMemory_[currentImage], 0, 2 * sizeof(VkDrawIndirectCommand), 0, (void **)&data);
	data[0] = { .vertexCount = 3, .instanceCount = 1, .firstVertex = 0, .firstInstance = 0 };
	data[1] = { .vertexCount = 3, .instanceCount = 1, .firstVertex = 3, .firstInstance = 1 };
	vkUnmapMemory(vkDev.device, indirectBuffersMemory_[currentImage]);
}

bool MultiRenderer::init(
	VulkanRenderDevice& vkDev,
	int maxVtxSize, int maxIdxSize,
	int numInstances,
	int instanceDataSize, int materialDataSize,
	const char* vertShaderFile, const char* fragShaderFile)
{
	if (!createColorAndDepthRenderPass(vkDev, false, &renderPass_, RenderPassCreateInfo()))
	{
		printf("Failed to create render pass\n");
		exit(EXIT_FAILURE);
	}

	framebufferWidth_ = vkDev.framebufferWidth;
	framebufferHeight_ = vkDev.framebufferHeight;

	maxInstanceSize_ = instanceDataSize;
	maxMaterialSize_ = materialDataSize;
	maxInstances_ = numInstances;

	const size_t indirectDataSize = maxInstances_ * sizeof(VkDrawIndirectCommand);

	createDepthResources(vkDev, framebufferWidth_, framebufferHeight_, depthTexture_);

	countBuffers_.resize(vkDev.swapchainImages.size());
	countBuffersMemory_.resize(vkDev.swapchainImages.size());

	instanceBuffers_.resize(vkDev.swapchainImages.size());
	instanceBuffersMemory_.resize(vkDev.swapchainImages.size());

	indirectBuffers_.resize(vkDev.swapchainImages.size());
	indirectBuffersMemory_.resize(vkDev.swapchainImages.size());

	if (!createBuffer(vkDev.device, vkDev.physicalDevice, materialDataSize,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		materialBuffer_, materialBufferMemory_))
	{
		printf("Cannot create material buffer\n");
		fflush(stdout);
		return false;
	}

	maxVertexBufferSize_ = maxVtxSize;
	maxIndexBufferSize_ = maxIdxSize;

	if (!createBuffer(vkDev.device, vkDev.physicalDevice, maxVtxSize + maxIdxSize,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		storageBuffer_, storageBufferMemory_))
	{
		printf("Cannot create vertex/index buffer\n"); fflush(stdout);
		return false;
	}

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		if (!createBuffer(vkDev.device, vkDev.physicalDevice, indirectDataSize,
			VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, // | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, /* for debugging we make it host-visible */
			indirectBuffers_[i], indirectBuffersMemory_[i]))
		{
			printf("Cannot create indirect buffer\n"); fflush(stdout);
			return false;
		}

		if (!createBuffer(vkDev.device, vkDev.physicalDevice, instanceDataSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, /* for debugging we make it host-visible */
			instanceBuffers_[i], instanceBuffersMemory_[i]))
		{
			printf("Cannot create instance buffer\n"); fflush(stdout);
			return false;
		}

		if (!createBuffer(vkDev.device, vkDev.physicalDevice, sizeof(uint32_t),
			VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, /* for debugging we make it host-visible */
			countBuffers_[i], countBuffersMemory_[i]))
		{
			printf("Cannot create count buffer\n");
			fflush(stdout);
			return false;
		}
	}

	if (!createUniformBuffers(vkDev, sizeof(mat4)))
		return false;

	if (!createColorAndDepthFramebuffers(vkDev, renderPass_, VK_NULL_HANDLE, swapchainFramebuffers_))
		return false;

	if (!createDescriptorPool(vkDev, 1, 4, 0, &descriptorPool_) ||
		!createDescriptorSet(vkDev) ||
		!createPipelineLayout(vkDev.device, descriptorSetLayout_, &pipelineLayout_) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_, { vertShaderFile, fragShaderFile }, &graphicsPipeline_))
	{
		printf("Failed to create pipeline\n"); fflush(stdout);
		exit(0);
	}

	return VK_SUCCESS;
}

void MultiRenderer::destroy(VkDevice device)
{
	vkDestroyBuffer(device, storageBuffer_, nullptr);
	vkFreeMemory(device, storageBufferMemory_, nullptr);

	for (size_t i = 0; i < swapchainFramebuffers_.size(); i++)
	{
		vkDestroyBuffer(device, instanceBuffers_[i], nullptr);
		vkFreeMemory(device, instanceBuffersMemory_[i], nullptr);

		vkDestroyBuffer(device, countBuffers_[i], nullptr);
		vkFreeMemory(device, countBuffersMemory_[i], nullptr);

		vkDestroyBuffer(device, indirectBuffers_[i], nullptr);
		vkFreeMemory(device, indirectBuffersMemory_[i], nullptr);
	}

	vkDestroyBuffer(device, materialBuffer_, nullptr);
	vkFreeMemory(device, materialBufferMemory_, nullptr);

	destroyVulkanImage(device, depthTexture_);
}
