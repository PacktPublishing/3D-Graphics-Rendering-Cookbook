#pragma once

#include "shared/vkRenderers/VulkanRendererBase.h"

#include <string>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

class VulkanQuadRenderer: public RendererBase
{
public:
	VulkanQuadRenderer(VulkanRenderDevice& vkDev, const std::vector<std::string>& textureFiles);
	virtual ~VulkanQuadRenderer();

	void updateBuffer(VulkanRenderDevice& vkDev, size_t i);
	void pushConstants(VkCommandBuffer commandBuffer, uint32_t textureIndex, const glm::vec2& offset);

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

	void quad(float x1, float y1, float x2, float y2);
	void clear();

private:
	bool createDescriptorSet(VulkanRenderDevice& vkDev);

	struct ConstBuffer
	{
		vec2     offset;
		uint32_t textureIndex;
	};

	struct VertexData
	{
		vec3 pos;
		vec2 tc;
	};

	VulkanRenderDevice& vkDev;

	std::vector<VertexData> quads_;

	size_t vertexBufferSize_;
	size_t indexBufferSize_;

	// 6. Storage Buffer with index and vertex data
	std::vector<VkBuffer> storageBuffers_;
	std::vector<VkDeviceMemory> storageBuffersMemory_;

	std::vector<VulkanImage> textures_;
	std::vector<VkSampler> textureSamplers_;
};
