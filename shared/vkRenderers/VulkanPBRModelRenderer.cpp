#include "shared/vkRenderers/VulkanPBRModelRenderer.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;

#include <glad/gl.h>
#include <gli/gli.hpp>
#include <gli/texture2d.hpp>
#include <gli/load_ktx.hpp>

static constexpr VkClearColorValue clearValueColor = { 1.0f, 1.0f, 1.0f, 1.0f };

bool PBRModelRenderer::createDescriptorSet(VulkanRenderDevice& vkDev, uint32_t uniformDataSize)
{
	const std::vector<VkDescriptorSetLayoutBinding> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),

		descriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),  // AO
		descriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),  // Emissive
		descriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),  // Albedo
		descriptorSetLayoutBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),  // MeR
		descriptorSetLayoutBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),  // Normal

		descriptorSetLayoutBinding(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),  // env
		descriptorSetLayoutBinding(9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),  // env_IRR

		descriptorSetLayoutBinding(10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)  // brdfLUT
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
		const VkDescriptorImageInfo  imageInfoAO       = { texAO_.sampler, texAO_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		const VkDescriptorImageInfo  imageInfoEmissive = { texEmissive_.sampler, texEmissive_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		const VkDescriptorImageInfo  imageInfoAlbedo   = { texAlbedo_.sampler, texAlbedo_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		const VkDescriptorImageInfo  imageInfoMeR      = { texMeR_.sampler, texMeR_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		const VkDescriptorImageInfo  imageInfoNormal   = { texNormal_.sampler, texNormal_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		const VkDescriptorImageInfo  imageInfoEnv      = { envMap_.sampler, envMap_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		const VkDescriptorImageInfo  imageInfoEnvIrr   = { envMapIrradiance_.sampler, envMapIrradiance_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		const VkDescriptorImageInfo  imageInfoBRDF     = { brdfLUT_.sampler, brdfLUT_.image.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		const std::array<VkWriteDescriptorSet, 11> descriptorWrites = {
			bufferWriteDescriptorSet(ds, &bufferInfo,  0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo2, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo3, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
			imageWriteDescriptorSet( ds, &imageInfoAO,       3),
			imageWriteDescriptorSet( ds, &imageInfoEmissive, 4),
			imageWriteDescriptorSet( ds, &imageInfoAlbedo,   5),
			imageWriteDescriptorSet( ds, &imageInfoMeR,      6),
			imageWriteDescriptorSet( ds, &imageInfoNormal,   7),

			imageWriteDescriptorSet( ds, &imageInfoEnv,      8),
			imageWriteDescriptorSet( ds, &imageInfoEnvIrr,   9),
			imageWriteDescriptorSet( ds, &imageInfoBRDF,     10)
		};

		vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return true;
}

void PBRModelRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	beginRenderPass(commandBuffer, currentImage);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(indexBufferSize_ / (sizeof(unsigned int))), 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
}

void PBRModelRenderer::updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const void* data, const size_t dataSize)
{
	uploadBufferData(vkDev, uniformBuffersMemory_[currentImage], 0, data, dataSize);
}

static void loadTexture(VulkanRenderDevice& vkDev, const char* fileName, VulkanTexture& texture)
{
	createTextureImage(vkDev, fileName, texture.image.image, texture.image.imageMemory);
	createImageView(vkDev.device, texture.image.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &texture.image.imageView);
	createTextureSampler(vkDev.device, &texture.sampler);
}

static void loadCubeMap(VulkanRenderDevice& vkDev, const char* fileName, VulkanTexture& cubemap, uint32_t mipLevels = 1)
{
	if (mipLevels > 1)
		createMIPCubeTextureImage(vkDev, fileName, mipLevels, cubemap.image.image, cubemap.image.imageMemory);
	else
		createCubeTextureImage(vkDev, fileName, cubemap.image.image, cubemap.image.imageMemory);

	createImageView(vkDev.device, cubemap.image.image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, &cubemap.image.imageView, VK_IMAGE_VIEW_TYPE_CUBE, 6, mipLevels);
	createTextureSampler(vkDev.device, &cubemap.sampler);
}

PBRModelRenderer::PBRModelRenderer(VulkanRenderDevice& vkDev,
	uint32_t uniformBufferSize,
	const char* modelFile,
	const char* texAOFile,
	const char* texEmissiveFile,
	const char* texAlbedoFile,
	const char* texMeRFile,
	const char* texNormalFile,
	const char* texEnvMapFile,
	const char* texIrrMapFile,
	VulkanImage depthTexture): RendererBase(vkDev, VulkanImage())
{
	depthTexture_ = depthTexture;

	// Resource loading part
	if (!createPBRVertexBuffer(vkDev, modelFile, &storageBuffer_, &storageBufferMemory_, &vertexBufferSize_, &indexBufferSize_))
	{
		printf("ModelRenderer: createPBRVertexBuffer() failed\n");
		exit(EXIT_FAILURE);
	}

	loadTexture(vkDev, texAOFile,       texAO_);
	loadTexture(vkDev, texEmissiveFile, texEmissive_);
	loadTexture(vkDev, texAlbedoFile,   texAlbedo_);
	loadTexture(vkDev, texMeRFile,      texMeR_);
	loadTexture(vkDev, texNormalFile,   texNormal_);

	// cube maps
	loadCubeMap(vkDev, texEnvMapFile, envMap_);
	loadCubeMap(vkDev, texIrrMapFile, envMapIrradiance_);

	gli::texture gliTex = gli::load_ktx("data/brdfLUT.ktx");
	glm::tvec3<GLsizei> extent(gliTex.extent(0));

	if (!createTextureImageFromData(vkDev, brdfLUT_.image.image, brdfLUT_.image.imageMemory,
		(uint8_t*)gliTex.data(0, 0, 0), extent.x, extent.y, VK_FORMAT_R16G16_SFLOAT))
	{
		printf("ModelRenderer: failed to load BRDF LUT texture \n");
		exit(EXIT_FAILURE);
	}

	createImageView(vkDev.device, brdfLUT_.image.image, VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, &brdfLUT_.image.imageView);
	createTextureSampler(vkDev.device, &brdfLUT_.sampler);

	if (	!createColorAndDepthRenderPass(vkDev, true, &renderPass_, RenderPassCreateInfo()) ||
		!createUniformBuffers(vkDev, uniformBufferSize) ||
		!createColorAndDepthFramebuffers(vkDev, renderPass_, depthTexture_.imageView, swapchainFramebuffers_) ||
		!createDescriptorPool(vkDev, 1, 2, 8, &descriptorPool_) ||
		!createDescriptorSet(vkDev, uniformBufferSize) ||
		!createPipelineLayout(vkDev.device, descriptorSetLayout_, &pipelineLayout_) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_,
			{ "data/shaders/chapter06/VK05_mesh.vert", "data/shaders/chapter06/VK05_mesh.frag" },
			&graphicsPipeline_))
	{
		printf("PBRModelRenderer: failed to create pipeline\n");
		exit(EXIT_FAILURE);
	}
}

PBRModelRenderer::~PBRModelRenderer()
{
	vkDestroyBuffer(device_, storageBuffer_, nullptr);
	vkFreeMemory(device_, storageBufferMemory_, nullptr);

	destroyVulkanTexture(device_, texAO_);
	destroyVulkanTexture(device_, texEmissive_);
	destroyVulkanTexture(device_, texAlbedo_);
	destroyVulkanTexture(device_, texMeR_);
	destroyVulkanTexture(device_, texNormal_);

	destroyVulkanTexture(device_,  envMap_);
	destroyVulkanTexture(device_,  envMapIrradiance_);

	destroyVulkanTexture(device_,  brdfLUT_);
}
