#include "shared/Utils.h"
#include "shared/UtilsMath.h"
#include "shared/UtilsVulkan.h"
#include "shared/vkRenderers/VulkanCube.h"
#include "shared/EasyProfilerWrapper.h"

bool CubeRenderer::createDescriptorSet(VulkanRenderDevice& vkDev)
{
	const std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
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
		const VkDescriptorImageInfo  imageInfo   = { textureSampler, texture.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		const std::array<VkWriteDescriptorSet, 2> descriptorWrites = {
			bufferWriteDescriptorSet(ds, &bufferInfo,  0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
			imageWriteDescriptorSet( ds, &imageInfo,   1)
		};

		vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return true;
}

void CubeRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	EASY_FUNCTION();

	beginRenderPass(commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, 36, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);
}

void CubeRenderer::updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const mat4& m)
{
	uploadBufferData(vkDev, uniformBuffersMemory_[currentImage], 0, glm::value_ptr(m), sizeof(mat4));
}

CubeRenderer::CubeRenderer(VulkanRenderDevice& vkDev, VulkanImage inDepthTexture, const char* textureFile)
: RendererBase(vkDev, inDepthTexture)
{
	// Resource loading
	createCubeTextureImage(vkDev, textureFile, texture.image, texture.imageMemory);

	createImageView(vkDev.device, texture.image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, &texture.imageView, VK_IMAGE_VIEW_TYPE_CUBE, 6);
	createTextureSampler(vkDev.device, &textureSampler);

	// Pipeline initialization
	if (!createColorAndDepthRenderPass(vkDev, true, &renderPass_, RenderPassCreateInfo()) ||
		!createUniformBuffers(vkDev, sizeof(mat4)) ||
		!createColorAndDepthFramebuffers(vkDev, renderPass_, depthTexture_.imageView, swapchainFramebuffers_) ||
		!createDescriptorPool(vkDev, 1, 0, 1, &descriptorPool_) ||
		!createDescriptorSet(vkDev) ||
		!createPipelineLayout(vkDev.device, descriptorSetLayout_, &pipelineLayout_) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_, { "data/shaders/chapter04/VKCube.vert", "data/shaders/chapter04/VKCube.frag" }, &graphicsPipeline_))
	{
		printf("CubeRenderer: failed to create pipeline\n");
		exit(EXIT_FAILURE);
	}
}

CubeRenderer::~CubeRenderer()
{
	vkDestroySampler(device_, textureSampler, nullptr);
	destroyVulkanImage(device_, texture);
}
