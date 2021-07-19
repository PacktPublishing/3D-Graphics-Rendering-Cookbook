#include "shared/vkRenderers/VulkanQuadRenderer.h"

static constexpr int MAX_QUADS = 256;

void VulkanQuadRenderer::clear() { quads_.clear(); }

void VulkanQuadRenderer::quad(float x1, float y1, float x2, float y2)
{
	VertexData v1 { { x1, y1, 0 }, { 0, 0 } };
	VertexData v2 { { x2, y1, 0 }, { 1, 0 } };
	VertexData v3 { { x2, y2, 0 }, { 1, 1 } };
	VertexData v4 { { x1, y2, 0 }, { 0, 1 } };

	quads_.push_back( v1 );
	quads_.push_back( v2 );
	quads_.push_back( v3 );

	quads_.push_back( v1 );
	quads_.push_back( v3 );
	quads_.push_back( v4 );
}

bool VulkanQuadRenderer::createDescriptorSet(VulkanRenderDevice& vkDev)
{
	const std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<uint32_t>(textures_.size()))
	};

	const VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device, &layoutInfo, nullptr, &descriptorSetLayout_));

	const std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(), descriptorSetLayout_);

	const VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool_,
		.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),
		.pSetLayouts = layouts.data()
	};

	descriptorSets_.resize(vkDev.swapchainImages.size());

	VK_CHECK(vkAllocateDescriptorSets(vkDev.device, &allocInfo, descriptorSets_.data()));

	std::vector<VkDescriptorImageInfo> textureDescriptors(textures_.size());
	for (size_t i = 0; i < textures_.size(); i++) {
		textureDescriptors[i] = VkDescriptorImageInfo {
			.sampler = textureSamplers_[i],
			.imageView = textures_[i].imageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
	}

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		const VkDescriptorBufferInfo bufferInfo = {
			.buffer = uniformBuffers_[i],
			.offset = 0,
			.range = sizeof(ConstBuffer)
		};
		const VkDescriptorBufferInfo bufferInfo2 = {
			.buffer = storageBuffers_[i],
			.offset = 0,
			.range = vertexBufferSize_
		};

		const std::array<VkWriteDescriptorSet, 3> descriptorWrites = {
			VkWriteDescriptorSet {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptorSets_[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufferInfo
			},
			VkWriteDescriptorSet {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptorSets_[i],
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.pBufferInfo = &bufferInfo2
			},
			VkWriteDescriptorSet {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptorSets_[i],
				.dstBinding = 2,
				.dstArrayElement = 0,
				.descriptorCount = static_cast<uint32_t>(textures_.size()),
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = textureDescriptors.data()
			},
		};
		vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return true;
}

void VulkanQuadRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	if (quads_.empty())
		return;

	beginRenderPass(commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(quads_.size()), 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
}

void VulkanQuadRenderer::updateBuffer(VulkanRenderDevice& vkDev, size_t i)
{
	uploadBufferData(vkDev, storageBuffersMemory_[i], 0, quads_.data(), quads_.size() * sizeof(VertexData));
}

void VulkanQuadRenderer::pushConstants(VkCommandBuffer commandBuffer, uint32_t textureIndex, const glm::vec2& offset)
{
	const ConstBuffer constBuffer = { offset, textureIndex };
	vkCmdPushConstants(commandBuffer, pipelineLayout_, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ConstBuffer), &constBuffer);
}

VulkanQuadRenderer::VulkanQuadRenderer(VulkanRenderDevice& vkDev, const std::vector<std::string>& textureFiles):
	vkDev(vkDev),
	RendererBase(vkDev, VulkanImage())
{
	const size_t imgCount = vkDev.swapchainImages.size();

	framebufferWidth_ = vkDev.framebufferWidth;
	framebufferHeight_ = vkDev.framebufferHeight;

	storageBuffers_.resize(imgCount);
	storageBuffersMemory_.resize(imgCount);

	vertexBufferSize_ = MAX_QUADS * 6 * sizeof(VertexData);

	for(size_t i = 0 ; i < imgCount ; i++)
	{
		if (!createBuffer(vkDev.device, vkDev.physicalDevice, vertexBufferSize_,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			storageBuffers_[i], storageBuffersMemory_[i]))
		{
			printf("Cannot create vertex buffer\n");
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
	}

	if (!createUniformBuffers(vkDev, sizeof(ConstBuffer)))
	{
		printf("Cannot create data buffers\n");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	const size_t numTextureFiles = textureFiles.size();

	textures_.resize(numTextureFiles);
	textureSamplers_.resize(numTextureFiles);
	for (size_t i = 0; i < numTextureFiles; i++)
	{
		printf("\rLoading texture %u...", unsigned(i));
		createTextureImage(vkDev, textureFiles[i].c_str(), textures_[i].image, textures_[i].imageMemory);
		createImageView(vkDev.device, textures_[i].image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &textures_[i].imageView);
		createTextureSampler(vkDev.device, &textureSamplers_[i]);
	}
	printf("\n");

	if (!createDepthResources(vkDev, vkDev.framebufferWidth, vkDev.framebufferHeight, depthTexture_) ||
		!createDescriptorPool(vkDev, 1, 1, 1 * static_cast<uint32_t>(textures_.size()), &descriptorPool_) ||
		!createDescriptorSet(vkDev) ||
		!createColorAndDepthRenderPass(vkDev, false, &renderPass_, RenderPassCreateInfo()) ||
		!createPipelineLayoutWithConstants(vkDev.device, descriptorSetLayout_, &pipelineLayout_, sizeof(ConstBuffer), 0) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_, { "data/shaders/chapter06/VK02_texture_array.vert", "data/shaders/chapter06/VK02_texture_array.frag" }, &graphicsPipeline_))
	{
		printf("Failed to create pipeline\n");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	createColorAndDepthFramebuffers(vkDev, renderPass_, VK_NULL_HANDLE/*depthTexture.imageView*/, swapchainFramebuffers_);
}

VulkanQuadRenderer::~VulkanQuadRenderer()
{
	VkDevice device = vkDev.device;

	for (size_t i = 0; i < storageBuffers_.size(); i++)
	{
		vkDestroyBuffer(device, storageBuffers_[i], nullptr);
		vkFreeMemory(device, storageBuffersMemory_[i], nullptr);
	}

	for (size_t i = 0; i < textures_.size(); i++)
	{
		vkDestroySampler(device, textureSamplers_[i], nullptr);
		destroyVulkanImage(device, textures_[i]);
	}

	destroyVulkanImage(device, depthTexture_);
}
