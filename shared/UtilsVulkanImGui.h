#pragma once

#include "shared/UtilsVulkanRendererBase.h"

class ImGuiRenderer: public RendererBase
{
public:
	explicit ImGuiRenderer(VulkanRenderDevice& vkDev);
	virtual ~ImGuiRenderer();

	virtual void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage) override;
	void updateBuffers(VulkanRenderDevice& vkDev, uint32_t currentImage, const ImDrawData* imguiDrawData);

private:
	const ImDrawData* drawData = nullptr;

	bool createDescriptorSet(VulkanRenderDevice& vkDev);

	// storage buffer with index and vertex data
	VkDeviceSize bufferSize_;
	std::vector<VkBuffer> storageBuffer_;
	std::vector<VkDeviceMemory> storageBufferMemory_;

	VkSampler fontSampler_;
	VulkanImage font_;
};
