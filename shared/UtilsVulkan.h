#pragma once

#include <array>
#include <functional>
#include <vector>

#define VK_NO_PROTOTYPES
#include <volk/volk.h>

#include "glslang_c_interface.h"

#define VK_CHECK(value) CHECK(value == VK_SUCCESS, __FILE__, __LINE__);
#define VK_CHECK_RET(value) if ( value != VK_SUCCESS ) { CHECK(false, __FILE__, __LINE__); return value; }
#define BL_CHECK(value) CHECK(value, __FILE__, __LINE__);

struct VulkanInstance final
{
	VkInstance instance;
	VkSurfaceKHR surface;
	VkDebugUtilsMessengerEXT messenger;
	VkDebugReportCallbackEXT reportCallback;
};

struct VulkanRenderDevice final
{
	uint32_t framebufferWidth;
	uint32_t framebufferHeight;

	VkDevice device;
	VkQueue graphicsQueue;
	VkPhysicalDevice physicalDevice;

	uint32_t graphicsFamily;

	VkSwapchainKHR swapchain;
	VkSemaphore semaphore;
	VkSemaphore renderSemaphore;

	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	// For chapter5/6etc (compute shaders)

	// Were we initialized with compute capabilities
	bool useCompute = false;

	// [may coincide with graphicsFamily]
	uint32_t computeFamily;
	VkQueue computeQueue;

	// a list of all queues (for shared buffer allocation)
	std::vector<uint32_t> deviceQueueIndices;
	std::vector<VkQueue> deviceQueues;

	VkCommandBuffer computeCommandBuffer;
	VkCommandPool computeCommandPool;
};

// Features we need for our Vulkan context
struct VulkanContextFeatures
{
	bool supportScreenshots_ = false;

	bool geometryShader_    = true;
	bool tessellationShader_ = false;

	bool vertexPipelineStoresAndAtomics_ = false;
	bool fragmentStoresAndAtomics_ = false;
};

/* To avoid breaking chapter 1-6 samples, we introduce a class which differs from VulkanInstance in that it has a ctor & dtor */
struct VulkanContextCreator
{
	VulkanContextCreator() = default;

	VulkanContextCreator(VulkanInstance& vk, VulkanRenderDevice& dev, void* window, int screenWidth, int screenHeight, const VulkanContextFeatures& ctxFeatures = VulkanContextFeatures());
	~VulkanContextCreator();

	VulkanInstance& instance;
	VulkanRenderDevice& vkDev;
};

struct SwapchainSupportDetails final
{
	VkSurfaceCapabilitiesKHR capabilities = {};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct ShaderModule final
{
	std::vector<unsigned int> SPIRV;
	VkShaderModule shaderModule = nullptr;
};

struct VulkanBuffer
{
	VkBuffer       buffer;
	VkDeviceSize   size;
	VkDeviceMemory memory;

	/* Permanent mapping to CPU address space (see VulkanResources::addBuffer) */
	void*          ptr;
};

struct VulkanImage final
{
	VkImage image = nullptr;
	VkDeviceMemory imageMemory = nullptr;
	VkImageView imageView = nullptr;
};

// Aggregate structure for passing around the texture data
struct VulkanTexture final
{
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	VkFormat format;

	VulkanImage image;
	VkSampler sampler;

	// Offscreen buffers require VK_IMAGE_LAYOUT_GENERAL && static textures have VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	VkImageLayout desiredLayout;
};

void CHECK(bool check, const char* fileName, int lineNumber);

bool setupDebugCallbacks(VkInstance instance, VkDebugUtilsMessengerEXT* messenger, VkDebugReportCallbackEXT* reportCallback);

VkResult createShaderModule(VkDevice device, ShaderModule* shader, const char* fileName);

size_t compileShaderFile(const char* file, ShaderModule& shaderModule);

inline VkPipelineShaderStageCreateInfo shaderStageInfo(VkShaderStageFlagBits shaderStage, ShaderModule& module, const char* entryPoint)
{
	return VkPipelineShaderStageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = shaderStage,
		.module = module.shaderModule,
		.pName = entryPoint,
		.pSpecializationInfo = nullptr
	};
}

VkShaderStageFlagBits glslangShaderStageToVulkan(glslang_stage_t sh);

inline VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount = 1)
{
	return VkDescriptorSetLayoutBinding{
		.binding = binding,
		.descriptorType = descriptorType,
		.descriptorCount = descriptorCount,
		.stageFlags = stageFlags,
		.pImmutableSamplers = nullptr
	};
}

inline VkWriteDescriptorSet bufferWriteDescriptorSet(VkDescriptorSet ds, const VkDescriptorBufferInfo* bi, uint32_t bindIdx, VkDescriptorType dType)
{
	return VkWriteDescriptorSet { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr,
		ds, bindIdx, 0, 1, dType, nullptr, bi, nullptr
	};
}

inline VkWriteDescriptorSet imageWriteDescriptorSet(VkDescriptorSet ds, const VkDescriptorImageInfo* ii, uint32_t bindIdx) 
{
	return VkWriteDescriptorSet { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr,
		ds, bindIdx, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		ii, nullptr, nullptr
	};
}

void createInstance(VkInstance* instance);

VkResult createDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures deviceFeatures, uint32_t graphicsFamily, VkDevice* device);

VkResult createSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t graphicsFamily, uint32_t width, uint32_t height, VkSwapchainKHR* swapchain, bool supportScreenshots = false);

size_t createSwapchainImages(VkDevice device, VkSwapchainKHR swapchain, std::vector<VkImage>& swapchainImages, std::vector<VkImageView>& swapchainImageViews);

VkResult createSemaphore(VkDevice device, VkSemaphore* outSemaphore);

bool createTextureSampler(VkDevice device, VkSampler* sampler, VkFilter minFilter = VK_FILTER_LINEAR, VkFilter maxFilter = VK_FILTER_LINEAR, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

bool createDescriptorPool(VulkanRenderDevice& vkDev, uint32_t uniformBufferCount, uint32_t storageBufferCount, uint32_t samplerCount, VkDescriptorPool* descriptorPool);

bool isDeviceSuitable(VkPhysicalDevice device);

SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

uint32_t chooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

VkResult findSuitablePhysicalDevice(VkInstance instance, std::function<bool(VkPhysicalDevice)> selector, VkPhysicalDevice* physicalDevice);

uint32_t findQueueFamilies(VkPhysicalDevice device, VkQueueFlags desiredFlags);

VkFormat findSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

VkFormat findDepthFormat(VkPhysicalDevice device);

bool hasStencilComponent(VkFormat format);

bool createGraphicsPipeline(
	VulkanRenderDevice& vkDev,
	VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
	const std::vector<const char*>& shaderFiles,
	VkPipeline* pipeline,
	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST /* defaults to triangles*/,
	bool useDepth = true,
	bool useBlending = true,
	bool dynamicScissorState = false,
	int32_t customWidth  = -1,
	int32_t customHeight = -1,
	uint32_t numPatchControlPoints = 0);

VkResult createComputePipeline(VkDevice device, VkShaderModule computeShader, VkPipelineLayout pipelineLayout, VkPipeline* pipeline);

bool createSharedBuffer(VulkanRenderDevice& vkDev, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

bool createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
bool createImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, VkImageCreateFlags flags = 0, uint32_t mipLevels = 1);

bool createVolume(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t depth,
	VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, VkImageCreateFlags flags);

bool createOffscreenImage(VulkanRenderDevice& vkDev,
		VkImage& textureImage, VkDeviceMemory& textureImageMemory,
		uint32_t texWidth, uint32_t texHeight,
		VkFormat texFormat,
		uint32_t layerCount, VkImageCreateFlags flags);

bool createOffscreenImageFromData(VulkanRenderDevice& vkDev,
		VkImage& textureImage, VkDeviceMemory& textureImageMemory,
		void* imageData, uint32_t texWidth, uint32_t texHeight,
		VkFormat texFormat,
		uint32_t layerCount, VkImageCreateFlags flags);

bool createDepthSampler(VkDevice device, VkSampler* sampler);

bool createUniformBuffer(VulkanRenderDevice& vkDev, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize bufferSize);

/** Copy [data] to GPU device buffer */
void uploadBufferData(VulkanRenderDevice& vkDev, const VkDeviceMemory& bufferMemory, VkDeviceSize deviceOffset, const void* data, const size_t dataSize);

/** Copy GPU device buffer data to [outData] */
void downloadBufferData(VulkanRenderDevice& vkDev, const VkDeviceMemory& bufferMemory, VkDeviceSize deviceOffset, void* outData, size_t dataSize);

bool createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t layerCount = 1, uint32_t mipLevels = 1);

enum eRenderPassBit : uint8_t
{
	eRenderPassBit_First     = 0x01,
	eRenderPassBit_Last      = 0x02,
	eRenderPassBit_Offscreen = 0x04,
	eRenderPassBit_OffscreenInternal = 0x08,
};

struct RenderPassCreateInfo final
{
	bool clearColor_ = false;
	bool clearDepth_ = false;
	uint8_t flags_ = 0;
};

// Utility structure for Renderer classes to know the details about starting this pass
struct RenderPass
{
	RenderPass() = default;
	explicit RenderPass(VulkanRenderDevice& device, bool useDepth = true, const RenderPassCreateInfo& ci = RenderPassCreateInfo());

	RenderPassCreateInfo info;
	VkRenderPass handle = VK_NULL_HANDLE;
};

bool createColorOnlyRenderPass(VulkanRenderDevice& device, VkRenderPass* renderPass, const RenderPassCreateInfo& ci, VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM);
bool createColorAndDepthRenderPass(VulkanRenderDevice& device, bool useDepth, VkRenderPass* renderPass, const RenderPassCreateInfo& ci, VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM);
bool createDepthOnlyRenderPass(VulkanRenderDevice& vkDev, VkRenderPass* renderPass, const RenderPassCreateInfo& ci);

VkCommandBuffer beginSingleTimeCommands(VulkanRenderDevice& vkDev);
void endSingleTimeCommands(VulkanRenderDevice& vkDev, VkCommandBuffer commandBuffer);
void copyBuffer(VulkanRenderDevice& vkDev, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void transitionImageLayout(VulkanRenderDevice& vkDev, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount = 1, uint32_t mipLevels = 1);
void transitionImageLayoutCmd(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount = 1, uint32_t mipLevels = 1);

bool initVulkanRenderDevice(VulkanInstance& vk, VulkanRenderDevice& vkDev, uint32_t width, uint32_t height, std::function<bool(VkPhysicalDevice)> selector, VkPhysicalDeviceFeatures deviceFeatures);
bool initVulkanRenderDevice2(VulkanInstance& vk, VulkanRenderDevice& vkDev, uint32_t width, uint32_t height, std::function<bool(VkPhysicalDevice)> selector, VkPhysicalDeviceFeatures2 deviceFeatures2);
bool initVulkanRenderDevice3(VulkanInstance& vk, VulkanRenderDevice& vkDev, uint32_t width, uint32_t height, const VulkanContextFeatures& ctxFeatures = VulkanContextFeatures());
void destroyVulkanRenderDevice(VulkanRenderDevice& vkDev);
void destroyVulkanInstance(VulkanInstance& vk);

bool initVulkanRenderDeviceWithCompute(VulkanInstance& vk, VulkanRenderDevice& vkDev, uint32_t width, uint32_t height, VkPhysicalDeviceFeatures deviceFeatures);

bool initVulkanRenderDevice2WithCompute(VulkanInstance& vk, VulkanRenderDevice& vkDev, uint32_t width, uint32_t height, std::function<bool(VkPhysicalDevice)> selector, VkPhysicalDeviceFeatures2 deviceFeatures2, bool supportScreenshots = false);

bool createColorAndDepthFramebuffers(VulkanRenderDevice& vkDev, VkRenderPass renderPass, VkImageView depthImageView, std::vector<VkFramebuffer>& swapchainFramebuffers);
bool createColorAndDepthFramebuffer(VulkanRenderDevice& vkDev,
	uint32_t width, uint32_t height,
	VkRenderPass renderPass, VkImageView colorImageView, VkImageView depthImageView,
	VkFramebuffer* framebuffer);
bool createDepthOnlyFramebuffer(VulkanRenderDevice& vkDev,
	uint32_t width, uint32_t height,
	VkRenderPass renderPass, VkImageView depthImageView,
	VkFramebuffer* framebuffer);

void copyBufferToImage(VulkanRenderDevice& vkDev, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount = 1);
void copyImageToBuffer(VulkanRenderDevice& vkDev, VkImage image, VkBuffer buffer, uint32_t width, uint32_t height, uint32_t layerCount = 1);

void copyMIPBufferToImage(VulkanRenderDevice& vkDev, VkBuffer buffer, VkImage image, uint32_t mipLevels, uint32_t width, uint32_t height, uint32_t bytesPP, uint32_t layerCount = 1);

void destroyVulkanImage(VkDevice device, VulkanImage& image);
void destroyVulkanTexture(VkDevice device, VulkanTexture& texture);

uint32_t bytesPerTexFormat(VkFormat fmt);

/* VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL for real update of an existing texture */
bool updateTextureImage(VulkanRenderDevice& vkDev, VkImage& textureImage, VkDeviceMemory& textureImageMemory, uint32_t texWidth, uint32_t texHeight, VkFormat texFormat, uint32_t layerCount, const void* imageData, VkImageLayout sourceImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);

bool updateTextureVolume(VulkanRenderDevice& vkDev, VkImage& textureVolume, VkDeviceMemory& textureVolumeMemory, uint32_t texWidth, uint32_t texHeight, uint32_t texDepth, VkFormat texFormat, const void* volumeData, VkImageLayout sourceImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);

bool downloadImageData(VulkanRenderDevice& vkDev, VkImage& textureImage, uint32_t texWidth, uint32_t texHeight, VkFormat texFormat, uint32_t layerCount, void* imageData, VkImageLayout sourceImageLayout);

bool createDepthResources(VulkanRenderDevice& vkDev, uint32_t width, uint32_t height, VulkanImage& depth);

bool createPipelineLayout(VkDevice device, VkDescriptorSetLayout dsLayout, VkPipelineLayout* pipelineLayout);

bool createPipelineLayoutWithConstants(VkDevice device, VkDescriptorSetLayout dsLayout, VkPipelineLayout* pipelineLayout, uint32_t vtxConstSize, uint32_t fragConstSize);

bool createTextureImageFromData(VulkanRenderDevice& vkDev,
		VkImage& textureImage, VkDeviceMemory& textureImageMemory,
		void* imageData, uint32_t texWidth, uint32_t texHeight,
		VkFormat texFormat,
		uint32_t layerCount = 1, VkImageCreateFlags flags = 0);

bool createMIPTextureImageFromData(VulkanRenderDevice& vkDev,
		VkImage& textureImage, VkDeviceMemory& textureImageMemory,
		void* mipData, uint32_t mipLevels, uint32_t texWidth, uint32_t texHeight,
		VkFormat texFormat,
		uint32_t layerCount = 1, VkImageCreateFlags flags = 0);

bool createTextureVolumeFromData(VulkanRenderDevice& vkDev,
		VkImage& textureVolume, VkDeviceMemory& textureVolumeMemory,
		void* volumeData, uint32_t texWidth, uint32_t texHeight, uint32_t texDepth,
		VkFormat texFormat,
		VkImageCreateFlags flags = 0);

bool createTextureImage(VulkanRenderDevice& vkDev, const char* filename, VkImage& textureImage, VkDeviceMemory& textureImageMemory, uint32_t* outTexWidth = nullptr, uint32_t* outTexHeight = nullptr);

bool createMIPTextureImage(VulkanRenderDevice& vkDev, const char* filename, uint32_t mipLevels, VkImage& textureImage, VkDeviceMemory& textureImageMemory, uint32_t* width = nullptr, uint32_t* height = nullptr);

bool createCubeTextureImage(VulkanRenderDevice& vkDev, const char* filename, VkImage& textureImage, VkDeviceMemory& textureImageMemory, uint32_t* width = nullptr, uint32_t* height = nullptr);

bool createMIPCubeTextureImage(VulkanRenderDevice& vkDev, const char* filename, uint32_t mipLevels, VkImage& textureImage, VkDeviceMemory& textureImageMemory, uint32_t* width = nullptr, uint32_t* height = nullptr);

size_t allocateVertexBuffer(VulkanRenderDevice& vkDev, VkBuffer* storageBuffer, VkDeviceMemory* storageBufferMemory, size_t vertexDataSize, const void* vertexData, size_t indexDataSize, const void* indexData);

bool createTexturedVertexBuffer(VulkanRenderDevice& vkDev, const char* filename, VkBuffer* storageBuffer, VkDeviceMemory* storageBufferMemory, size_t* vertexBufferSize, size_t* indexBufferSize);

bool createPBRVertexBuffer(VulkanRenderDevice& vkDev, const char* filename, VkBuffer* storageBuffer, VkDeviceMemory* storageBufferMemory, size_t* vertexBufferSize, size_t* indexBufferSize);

bool executeComputeShader(VulkanRenderDevice& vkDev,
	VkPipeline computePipeline, VkPipelineLayout pl, VkDescriptorSet ds,
	uint32_t xsize, uint32_t ysize, uint32_t zsize);

bool createComputeDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout* descriptorSetLayout);

void insertComputedBufferBarrier(VulkanRenderDevice& vkDev, VkCommandBuffer commandBuffer, VkBuffer buffer);
void insertComputedImageBarrier(VkCommandBuffer commandBuffer, VkImage image);

VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physDevice);

inline uint32_t getVulkanBufferAlignment(VulkanRenderDevice& vkDev)
{
	VkPhysicalDeviceProperties devProps;
        vkGetPhysicalDeviceProperties(vkDev.physicalDevice, &devProps);
	return static_cast<uint32_t>(devProps.limits.minStorageBufferOffsetAlignment);
}

/* Check if the texture is used as a depth buffer */
inline bool isDepthFormat(VkFormat fmt) {
	return
		(fmt == VK_FORMAT_D16_UNORM) ||
		(fmt == VK_FORMAT_X8_D24_UNORM_PACK32) ||
		(fmt == VK_FORMAT_D32_SFLOAT) ||
		(fmt == VK_FORMAT_D16_UNORM_S8_UINT) ||
		(fmt == VK_FORMAT_D24_UNORM_S8_UINT) ||
		(fmt == VK_FORMAT_D32_SFLOAT_S8_UINT);
}

bool setVkObjectName(VulkanRenderDevice& vkDev, void* object, VkObjectType objType, const char* name);

inline bool setVkImageName(VulkanRenderDevice& vkDev, void* object, const char* name)
{
	return setVkObjectName(vkDev, object, VK_OBJECT_TYPE_IMAGE, name);
}

/* This routine updates one texture discriptor in one descriptor set */
void updateTextureInDescriptorSetArray(VulkanRenderDevice& vkDev, VkDescriptorSet ds, VulkanTexture t, uint32_t textureIndex, uint32_t bindingIdx);
