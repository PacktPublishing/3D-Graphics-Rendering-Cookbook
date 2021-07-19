#pragma once

#include "shared/Utils.h"
#include "shared/UtilsVulkan.h"

struct ComputeBase
{
	ComputeBase(VulkanRenderDevice& vkDev, const char* shaderName, uint32_t inputSize, uint32_t outputSize);

	virtual ~ComputeBase();

	inline void uploadInput(uint32_t offset, void* inData, uint32_t byteCount) {
		uploadBufferData(vkDev, inBufferMemory, offset, inData, byteCount);
	}

	inline void downloadOutput(uint32_t offset, void* outData, uint32_t byteCount) {
		downloadBufferData(vkDev, outBufferMemory, offset, outData, byteCount);
	}

	inline bool execute(uint32_t xsize, uint32_t ysize, uint32_t zsize) {
		return executeComputeShader(vkDev, pipeline, pipelineLayout, descriptorSet, xsize, ysize, zsize);
	}

protected:
	VulkanRenderDevice& vkDev;

	VkBuffer inBuffer;
	VkBuffer outBuffer;
	VkDeviceMemory inBufferMemory;
	VkDeviceMemory outBufferMemory;

	VkDescriptorSetLayout dsLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	bool createComputeDescriptorSet(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);
};
