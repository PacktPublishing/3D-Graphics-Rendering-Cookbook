#pragma once

#include "shared/UtilsVulkanRendererBase.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

class VulkanQuadRenderer: public RendererBase
{
public:
	bool init(VulkanRenderDevice& vkDev, const std::vector<const char*> textureFiles);
	void destroy(VkDevice device);

	void updateBuffer(VulkanRenderDevice& vkDev, size_t i);
	void updateUniformBuffer(VulkanRenderDevice& vkDev, uint32_t currentImage, const glm::mat4& m, float time);

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;

	void quad(float x1, float y1, float x2, float y2, int texIdx);
	void clear();

private:
	bool createDescriptorSet(VulkanRenderDevice& vkDev);

	struct UniformBuffer
	{
		mat4 mvp;
		int time;
	};

	struct VertexData
	{
		vec3 pos;
		vec2 tc;
		int texIdx;
	};

	std::vector<VertexData> quads_;

	size_t vertexBufferSize_;
	size_t indexBufferSize_;

	// 6. Storage Buffer with index and vertex data
	std::vector<VkBuffer> storageBuffers_;
	std::vector<VkDeviceMemory> storageBuffersMemory_;

	std::vector<VulkanImage> textures_;
	std::vector<VkSampler> textureSamplers_;
};
