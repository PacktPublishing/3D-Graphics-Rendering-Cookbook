#pragma once

#include "shared/vkFramework/VulkanShaderProcessor.h"

struct HDRPostprocessor: public QuadProcessor
{
	HDRPostprocessor(VulkanRenderContext& ctx, VulkanTexture input,
		VulkanTexture output):
		QuadProcessor(ctx, DescriptorSetInfo {
			.buffers = { mappedUniformBufferAttachment(ctx.resources, &ubo, VK_SHADER_STAGE_FRAGMENT_BIT) },
			.textures = { fsTextureAttachment(input) }
		},
		{ output }, "data/shaders/chapter08/VK_ToneMap.frag")
	{}

	inline float* getGammaPtr() { return &ubo->gamma; }
	inline float* getExposurePtr() { return &ubo->exposure; }

private:
	struct UniformBuffer {
		float gamma = 0.7f;
		float exposure = 2.5f;
	} *ubo;
};
