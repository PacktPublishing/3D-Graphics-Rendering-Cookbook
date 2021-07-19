#include "shared/vkRenderers/VulkanSingleQuad.h"
#include <stdio.h>

bool VulkanSingleQuadRenderer::createDescriptorSet(VulkanRenderDevice& vkDev, VkImageLayout desiredLayout)
{
	VkDescriptorSetLayoutBinding binding =
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	const VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = 1,
		.pBindings = &binding
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

	VkDescriptorImageInfo textureDescriptor = VkDescriptorImageInfo {
		.sampler = textureSampler,
		.imageView = texture.imageView,
		.imageLayout = desiredLayout
	};

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		VkWriteDescriptorSet imageDescriptorWrite = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets_[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &textureDescriptor
		};

		vkUpdateDescriptorSets(vkDev.device, 1, &imageDescriptorWrite, 0, nullptr);
	}

	return true;
}

void VulkanSingleQuadRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	beginRenderPass(commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, 6, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
}

VulkanSingleQuadRenderer::VulkanSingleQuadRenderer(VulkanRenderDevice& vkDev, VulkanImage tex, VkSampler sampler, VkImageLayout desiredLayout)
	: vkDev(vkDev)
	, texture(tex)
	, textureSampler(sampler)
	, RendererBase(vkDev, VulkanImage())
{
	/* we don't need them, but allocate them to allow destructor to complete */
	if (!createUniformBuffers(vkDev, sizeof(uint32_t)) ||
		!createDescriptorPool(vkDev, 0, 0, 1, &descriptorPool_) ||
		!createDescriptorSet(vkDev, desiredLayout) ||
		!createColorAndDepthRenderPass(vkDev, false, &renderPass_, RenderPassCreateInfo()) ||
		!createPipelineLayout(vkDev.device, descriptorSetLayout_, &pipelineLayout_) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_, { "data/shaders/chapter06/VK03_quad.vert", "data/shaders/chapter06/VK03_quad.frag" }, &graphicsPipeline_))
	{
		printf("Failed to create pipeline\n");
		fflush(stdout);
		exit(0);
	}

	createColorAndDepthFramebuffers(vkDev, renderPass_, VK_NULL_HANDLE, swapchainFramebuffers_);
}

VulkanSingleQuadRenderer::~VulkanSingleQuadRenderer()
{
}
