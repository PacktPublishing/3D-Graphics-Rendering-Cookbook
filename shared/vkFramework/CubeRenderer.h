#pragma once

#include "shared/vkFramework/Renderer.h"

struct CubemapRenderer: public Renderer
{
	CubemapRenderer(VulkanRenderContext& ctx,
		VulkanTexture texture,
		const std::vector<VulkanTexture>& outputs = std::vector<VulkanTexture> {},
		RenderPass screenRenderPass = RenderPass());

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override;

	void updateBuffers(size_t currentImage) override;

	inline void setMatrices(const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model) { ubo.proj_ = proj; ubo.view_ = view; ubo.model_ = model; }

private:
	struct UniformBuffer
	{
		glm::mat4 proj_;
		glm::mat4 view_;
		glm::mat4 model_;
	} ubo;
};
