#include "shared/vkRenderers/VulkanComputedImage.h"

ComputedImage::ComputedImage(VulkanRenderDevice& vkDev, const char* shaderName, uint32_t textureWidth, uint32_t textureHeight, bool supportDownload)
	: ComputedItem(vkDev, sizeof(uint32_t))
	, computedWidth(textureWidth)
	, computedHeight(textureHeight)
	, canDownloadImage(supportDownload)
{
	createComputedTexture(textureWidth, textureHeight);
	createComputedImageSetLayout();

	const VkPushConstantRange pushConstantRange = {
	 .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
	 .offset = 0,
	 .size = sizeof(float)
	};

	const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.setLayoutCount = 1,
	.pSetLayouts = &dsLayout,
	.pushConstantRangeCount = 1,
	.pPushConstantRanges = &pushConstantRange
	};

	VK_CHECK(vkCreatePipelineLayout(vkDev.device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

	createDescriptorSet();

	ShaderModule s;
	createShaderModule(vkDev.device, &s, shaderName);
	if (createComputePipeline(vkDev.device, s.shaderModule, pipelineLayout, &pipeline) != VK_SUCCESS)
		exit(EXIT_FAILURE);

	vkDestroyShaderModule(vkDev.device, s.shaderModule, nullptr);
}

bool ComputedImage::createComputedTexture(uint32_t computedWidth, uint32_t computedHeight, VkFormat format)
{
	VkFormatProperties fmtProps;
	vkGetPhysicalDeviceFormatProperties(vkDev.physicalDevice, format, &fmtProps);
	if (!(fmtProps.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
		return false;

	if (!createImage(vkDev.device, vkDev.physicalDevice,
		computedWidth, computedHeight, format,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | (canDownloadImage ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0),
		!canDownloadImage ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
		computed.image, computed.imageMemory))
		return false;

	transitionImageLayout(vkDev, computed.image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	return 	createTextureSampler(vkDev.device, &computedImageSampler) &&
		createImageView(vkDev.device, computed.image, format, VK_IMAGE_ASPECT_COLOR_BIT, &computed.imageView);
}

bool ComputedImage::createComputedImageSetLayout()
{
	std::vector<VkDescriptorPoolSize> poolSizes =
	{
		{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1 },
		{ .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,  .descriptorCount = 1 }
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
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,  VK_SHADER_STAGE_COMPUTE_BIT),
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

bool ComputedImage::createDescriptorSet()
{
	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &dsLayout
	};

	VK_CHECK(vkAllocateDescriptorSets(vkDev.device, &allocInfo, &descriptorSet));

	const VkDescriptorBufferInfo bufferInfo  = { uniformBuffer.buffer, 0, uniformBuffer.size };
	const VkDescriptorImageInfo  imageInfo   = { computedImageSampler, computed.imageView, VK_IMAGE_LAYOUT_GENERAL/*VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/ };

	std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
		VkWriteDescriptorSet { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .pNext = nullptr,
			.dstSet = descriptorSet, .dstBinding = 0, .dstArrayElement = 0, .descriptorCount = 1, .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			.pImageInfo = &imageInfo, .pBufferInfo = nullptr, .pTexelBufferView = nullptr
		},
		bufferWriteDescriptorSet(descriptorSet, &bufferInfo, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
	};

	vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

	return true;
}

void ComputedImage::downloadImage(void* imageData)
{
	if (!canDownloadImage || !imageData)
		return;

	downloadImageData(vkDev, computed.image, computedWidth, computedHeight, VK_FORMAT_R8G8B8A8_UNORM, 1, imageData, VK_IMAGE_LAYOUT_GENERAL);
}
