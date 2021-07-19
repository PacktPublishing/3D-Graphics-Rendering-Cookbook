#pragma once

#include "shared/Utils.h"
#include "shared/vkRenderers/VulkanComputedItem.h"

struct ComputedVertexBuffer: public ComputedItem
{
	ComputedVertexBuffer(VulkanRenderDevice& vkDev, const char* shaderName,
		uint32_t indexBufferSize,
		uint32_t uniformBufferSize,
		uint32_t vertexSize,
		uint32_t vertexCount,
		bool supportDownload = false);

	virtual ~ComputedVertexBuffer() {}

	void uploadIndexData(uint32_t* indices);

	void downloadVertices(void* vertexData);

	VkBuffer computedBuffer;
	VkDeviceMemory computedMemory;

	uint32_t computedVertexCount;

protected:
	uint32_t indexBufferSize;
	uint32_t vertexSize;

	bool canDownloadVertices;

	bool createComputedBuffer();

	bool createDescriptorSet();
	bool createComputedSetLayout();
};
