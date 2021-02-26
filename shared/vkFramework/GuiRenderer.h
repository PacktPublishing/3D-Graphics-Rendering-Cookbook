#pragma once

#include "shared/vkFramework/Renderer.h"

struct Scene;

struct GuiRenderer: public Renderer
{
	GuiRenderer(VulkanRenderContext& ctx, const std::vector<VulkanTexture>& textures = std::vector<VulkanTexture> {}, RenderPass renderPass = RenderPass());
	virtual ~GuiRenderer();

	void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override;
	void updateBuffers(size_t currentImage) override;

private:
	std::vector<VulkanTexture> allTextures;

	// storage buffer with index and vertex data
	std::vector<VulkanBuffer> storages_;
};

void imguiTextureWindow(const char* Title, uint32_t texId);
int renderSceneTree(const Scene& scene, int node);
