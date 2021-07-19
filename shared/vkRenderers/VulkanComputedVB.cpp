#include "shared/vkRenderers/VulkanComputedVB.h"

ComputedVertexBuffer::ComputedVertexBuffer(VulkanRenderDevice& vkDev, const char* shaderName,
	uint32_t indexBufferSize,
	uint32_t uniformBufferSize,
	uint32_t vertexSize,
	uint32_t vertexCount,
	bool supportDownload)
	: ComputedItem(vkDev, uniformBufferSize)
	, computedVertexCount(vertexCount)
	, indexBufferSize(indexBufferSize)
	, vertexSize(vertexSize)
	, canDownloadVertices(supportDownload)
{
	createComputedBuffer();
	createComputedSetLayout();
	createPipelineLayout(vkDev.device, dsLayout, &pipelineLayout);

	createDescriptorSet();

	ShaderModule s;
	createShaderModule(vkDev.device, &s, shaderName);
	if (createComputePipeline(vkDev.device, s.shaderModule, pipelineLayout, &pipeline) != VK_SUCCESS)
		exit(EXIT_FAILURE);

	vkDestroyShaderModule(vkDev.device, s.shaderModule, nullptr);
}

bool ComputedVertexBuffer::createComputedBuffer()
{
	return createBuffer(vkDev.device, vkDev.physicalDevice,
		computedVertexCount * vertexSize + indexBufferSize,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | (canDownloadVertices ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		!canDownloadVertices? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
		computedBuffer, computedMemory);
}

bool ComputedVertexBuffer::createComputedSetLayout()
{
	std::vector<VkDescriptorPoolSize> poolSizes =
	{
		{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1 },
		{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1 }
	};

	VkDescriptorPoolCreateInfo descriptorPoolInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = 1,
		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		.pPoolSizes    = poolSizes.data()
	};

	VK_CHECK(vkCreateDescriptorPool(vkDev.device, &descriptorPoolInfo, nullptr, &descriptorPool));

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT),
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
	};

	const VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device, &layoutInfo, nullptr, &dsLayout));

	return true;
}

bool ComputedVertexBuffer::createDescriptorSet()
{
	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &dsLayout
	};

	VK_CHECK(vkAllocateDescriptorSets(vkDev.device, &allocInfo, &descriptorSet));

	const VkDescriptorBufferInfo bufferInfo  = { computedBuffer,        0, computedVertexCount * vertexSize };
	const VkDescriptorBufferInfo bufferInfo2 = { uniformBuffer.buffer,  0, uniformBuffer.size };

	std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
		bufferWriteDescriptorSet(descriptorSet, &bufferInfo,  0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
		bufferWriteDescriptorSet(descriptorSet, &bufferInfo2, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
	};

	vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

	return true;
}

void ComputedVertexBuffer::downloadVertices(void* vertexData)
{
	if (!canDownloadVertices || !vertexData)
		return;

	downloadBufferData(vkDev, computedMemory, 0, vertexData, computedVertexCount * vertexSize);
}

void ComputedVertexBuffer::uploadIndexData(uint32_t* indices)
{
	uploadBufferData(vkDev, computedMemory, computedVertexCount * vertexSize, indices, indexBufferSize);
}
