#pragma once
#include "shared/vkFramework/VulkanApp.h"

/// A collection of renderers acting as one renderer (for Screen-Space effects in Chapter8)
struct CompositeRenderer: public Renderer
{
	CompositeRenderer(VulkanRenderContext& c): Renderer(c)
	{}

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb1 = VK_NULL_HANDLE, VkRenderPass rp1 = VK_NULL_HANDLE) override
	{
		for (auto& r: renderers_)
		if (r.enabled_)
		{
			VkRenderPass rp = rp1;
			VkFramebuffer fb = fb1;

			if (r.renderer_.renderPass_.handle != VK_NULL_HANDLE)
				rp = r.renderer_.renderPass_.handle;
			if (r.renderer_.framebuffer_ != VK_NULL_HANDLE)
				fb = r.renderer_.framebuffer_;

			r.renderer_.fillCommandBuffer(cmdBuffer, currentImage, fb, rp);
		}
	}

	void updateBuffers(size_t currentImage) override
	{
		for (auto& r: renderers_)
			r.renderer_.updateBuffers(currentImage);
	}

protected:
	// A list of internal renderers
	std::vector<RenderItem> renderers_;
};
