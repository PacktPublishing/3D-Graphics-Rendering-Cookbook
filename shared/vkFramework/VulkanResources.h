#pragma once
#include "shared/UtilsVulkan.h"
#include <volk/volk.h>

#include <cstring>
#include <memory>
#include <map>
#include <utility>

/**
	For more or less abstract descriptor set setup we need to describe individual items ("bindings").
	These are buffers, textures (samplers, but we call them "textures" here) and arrays of textures.
*/

/// Common information for all bindings (buffer/sampler type and shader stage(s) where this item is used)
struct DescriptorInfo
{
	VkDescriptorType type;
	VkShaderStageFlags shaderStageFlags;
};

struct BufferAttachment
{
	DescriptorInfo dInfo;

	VulkanBuffer   buffer;
	uint32_t       offset;
	uint32_t       size;
};

struct TextureAttachment
{
	DescriptorInfo dInfo;

	VulkanTexture  texture;
};

struct TextureArrayAttachment
{
	DescriptorInfo dInfo;

	std::vector<VulkanTexture> textures;
};

struct TextureCreationParams
{
	VkFilter minFilter;
	VkFilter maxFilter;

///	VkAccessModeFlags accessMode;
};

inline TextureAttachment makeTextureAttachment(VulkanTexture tex, VkShaderStageFlags shaderStageFlags) {
	return TextureAttachment {
		.dInfo = {
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.shaderStageFlags = shaderStageFlags
		},
		.texture = tex
	};
}

inline TextureAttachment fsTextureAttachment(VulkanTexture tex) {
	return makeTextureAttachment(tex, VK_SHADER_STAGE_FRAGMENT_BIT);
}

inline TextureArrayAttachment fsTextureArrayAttachment(const std::vector<VulkanTexture>& textures) {
	return TextureArrayAttachment {
		.dInfo = {
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
		},
		.textures = textures
	};
}

inline BufferAttachment makeBufferAttachment(VulkanBuffer buffer, uint32_t offset, uint32_t size, VkDescriptorType type, VkShaderStageFlags shaderStageFlags) {
	return BufferAttachment {
		.dInfo = {
			.type = type,
			.shaderStageFlags = shaderStageFlags
		},
		.buffer = {
			.buffer = buffer.buffer,
			.size = buffer.size,
			.memory = buffer.memory,
			.ptr = buffer.ptr
		},
		.offset = offset,
		.size = size
	};
}

inline BufferAttachment uniformBufferAttachment(VulkanBuffer buffer, uint32_t offset, uint32_t size, VkShaderStageFlags shaderStageFlags) {
	return makeBufferAttachment(buffer, offset, size, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, shaderStageFlags);
}

inline BufferAttachment storageBufferAttachment(VulkanBuffer buffer, uint32_t offset, uint32_t size, VkShaderStageFlags shaderStageFlags) {
	return makeBufferAttachment(buffer, offset, size, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, shaderStageFlags);
}

/** An aggregate structure with all the data for descriptor set (or descriptor set layout) allocation */
struct DescriptorSetInfo
{
	std::vector<BufferAttachment>       buffers;
	std::vector<TextureAttachment>      textures;
	std::vector<TextureArrayAttachment> textureArrays;
};

/* A structure with pipeline parameters */
struct PipelineInfo
{
	uint32_t width  = 0;
	uint32_t height = 0;

	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST /* defaults to triangles*/;

	bool useDepth = true;

	bool useBlending = true;

	bool dynamicScissorState = false;

	uint32_t patchControlPoints = 0;
};

/**
	The VulkanResources class keeps track of all allocated/created Vulkan-related objects

 	The list of textures, buffers and descriptor pools.

	Individual methods from the load*() serie implement all loading 

	The postprocessing chain is a "mixture" of a rendering technique, fullscreen postprocessing pipeline, resource manager and a frame graph

	Since RenderingTechnique, FrameGraph and PostprocessingPipeline all directly fill the list of rendering API commands,
	it is natural to generalize all rendering operations here.

	The "resources" part of this class is a set of methods to ease allocation and loading of textures and buffers.
	All allocated/loaded textures and buffers are automatically destroyed in the end.

	On top of that class we may add a (pretty simple even without reflection) serialization routine
	which reads a list of used buffers/textures, all the processing/rendering steps, internal parameter names
	and inter-stage dependencies.
*/
struct VulkanResources
{
	VulkanResources(VulkanRenderDevice& vkDev): vkDev(vkDev) {}
	~VulkanResources();

	VulkanTexture loadTexture2D(const char* filename);

	VulkanTexture loadCubeMap(const char* fileName, uint32_t mipLevels = 1);

	VulkanTexture loadKTX(const char* fileName);

	VulkanTexture createFontTexture(const char* fontFile);

	VulkanTexture addColorTexture(int texWidth = 0, int texHeight = 0, VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM, VkFilter minFilter = VK_FILTER_LINEAR, VkFilter maxFilter = VK_FILTER_LINEAR, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

	VulkanTexture addDepthTexture(int texWidth = 0, int texHeight = 0, VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	VulkanTexture addSolidRGBATexture(uint32_t color = 0xFFFFFFFF);

	VulkanTexture addRGBATexture(int texWidth, int texHeight, void* data);

	VulkanBuffer addBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool createMapping = false);

	inline VulkanBuffer addUniformBuffer(VkDeviceSize bufferSize, bool createMapping = false) {
		return addBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, createMapping);
	}

	inline VulkanBuffer addIndirectBuffer(VkDeviceSize bufferSize, bool createMapping = false) {
		return addBuffer(bufferSize, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, // | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, createMapping); /* for debugging we make it host-visible */
	}

	inline VulkanBuffer addStorageBuffer(VkDeviceSize bufferSize, bool createMapping = false) {
		return addBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, createMapping); /* for debugging we make it host-visible */
	}

	inline VulkanBuffer addComputedIndirectBuffer(VkDeviceSize bufferSize, bool createMapping = false) {
		return addBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, createMapping); /* for debugging we make it host-visible */
	}

	/* Allocate and upload vertex & index buffer pair */
	VulkanBuffer addVertexBuffer(uint32_t indexBufferSize, const void* indexData, uint32_t vertexBufferSize, const void* vertexData);

	VkFramebuffer addFramebuffer(RenderPass renderPass, const std::vector<VulkanTexture>& images);

	RenderPass addFullScreenPass(bool useDepth = true, const RenderPassCreateInfo& ci = RenderPassCreateInfo());

	RenderPass addRenderPass(const std::vector<VulkanTexture>& outputs, const RenderPassCreateInfo& ci = {
		.clearColor_ = true, .clearDepth_ = true,
		.flags_ = eRenderPassBit_Offscreen | eRenderPassBit_First }, bool useDepth = true);

	RenderPass addDepthRenderPass(const std::vector<VulkanTexture>& outputs, const RenderPassCreateInfo& ci = {
		.clearColor_ = false, .clearDepth_ = true,
		.flags_ = eRenderPassBit_Offscreen | eRenderPassBit_First });

	VkPipelineLayout addPipelineLayout(VkDescriptorSetLayout dsLayout, uint32_t vtxConstSize = 0, uint32_t fragConstSize = 0);

	VkPipeline addPipeline(VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
		const std::vector<const char*>& shaderFiles,
		const PipelineInfo& pipelineParams = PipelineInfo { .width = 0, .height = 0, .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, .useDepth = true, .useBlending = false, .dynamicScissorState = false });

	VkPipeline addComputePipeline(const char* shaderFile, VkPipelineLayout pipelineLayout);

	/* Calculate the descriptor pool size from the list of buffers and textures */
	VkDescriptorPool addDescriptorPool(const DescriptorSetInfo& dsInfo, uint32_t dSetCount = 1);

	VkDescriptorSetLayout addDescriptorSetLayout(const DescriptorSetInfo& dsInfo);

	VkDescriptorSet addDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout dsLayout);

	void updateDescriptorSet(VkDescriptorSet ds, const DescriptorSetInfo& dsInfo);

	const std::vector<VulkanTexture>& getTextures() const { return allTextures; } 

	std::vector<VkFramebuffer> addFramebuffers(VkRenderPass renderPass, VkImageView depthView = VK_NULL_HANDLE);

	/**  Helper functions for small Chapter 8/9 demos */
	std::pair<BufferAttachment, BufferAttachment> makeMeshBuffers(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

	std::pair<BufferAttachment, BufferAttachment> loadMeshToBuffer(const char* filename, bool useTextureCoordinates, bool useNormals,
		std::vector<float>& vertices,
		std::vector<unsigned int>& indices);

	std::pair<BufferAttachment, BufferAttachment> createPlaneBuffer_XZ(float sx, float sz);
	std::pair<BufferAttachment, BufferAttachment> createPlaneBuffer_XY(float sx, float sy);

private:
	VulkanRenderDevice& vkDev;

	std::vector<VulkanTexture> allTextures;
	std::vector<VulkanBuffer> allBuffers;

	std::vector<VkFramebuffer> allFramebuffers;
	std::vector<VkRenderPass> allRenderPasses;

	std::vector<VkPipelineLayout> allPipelineLayouts;
	std::vector<VkPipeline> allPipelines;

	std::vector<VkDescriptorSetLayout> allDSLayouts;
	std::vector<VkDescriptorPool>      allDPools;

	std::vector<ShaderModule> shaderModules;
	std::map<std::string, int> shaderMap;

	bool createGraphicsPipeline(
		VulkanRenderDevice& vkDev,
		VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
		const std::vector<const char*>& shaderFiles,
		VkPipeline* pipeline,
		VkPrimitiveTopology topology,
		bool useDepth,
		bool useBlending,
		bool dynamicScissorState,
		int32_t customWidth,
		int32_t customHeight,
		uint32_t numPatchControlPoints);
};

/* A helper function for inplace allocation of VulkanBuffers. Helpful to avoid multiline buffer initialization in constructors */
inline VulkanBuffer bufferAllocator(VulkanResources& resources, VulkanBuffer& buffer, VkDeviceSize size, bool createMapping = false) {
	return (buffer = resources.addUniformBuffer(size, createMapping));
}

/* Create a uniform buffer mapped to a CPU location and initialize the buffer with default values */
template <class BufferT>
inline BufferAttachment mappedUniformBufferAttachment(VulkanResources& resources, BufferT** ptr, VkShaderStageFlags shaderStageFlags) {
	VulkanBuffer buffer = resources.addUniformBuffer(sizeof(BufferT), true);
	*ptr = (BufferT *)buffer.ptr;
	*(*ptr) = BufferT();
	return uniformBufferAttachment(buffer, 0, 0, shaderStageFlags);
}
