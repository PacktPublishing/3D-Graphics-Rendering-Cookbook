#pragma once
#include "shared/vkFramework/effects/LuminanceCalculator.h"

// Apply bloom to input buffer
struct BloomProcessor: public CompositeRenderer
{
	BloomProcessor(VulkanRenderContext& c, VulkanTexture input, VulkanTexture avgLuminance): CompositeRenderer(c),

		bloomUniforms(c.resources.addUniformBuffer(sizeof(UniformBuffer))),

		brightnessTex(c.resources.addColorTexture(0, 0, LuminosityFormat)),

		bloomX1Tex(c.resources.addColorTexture(0, 0, LuminosityFormat)),
		bloomY1Tex(c.resources.addColorTexture(0, 0, LuminosityFormat)),
		bloomX2Tex(c.resources.addColorTexture(0, 0, LuminosityFormat)),
		bloomY2Tex(c.resources.addColorTexture(0, 0, LuminosityFormat)),

		streaks1Tex(c.resources.addColorTexture(0, 0, LuminosityFormat)),
		streaks2Tex(c.resources.addColorTexture(0, 0, LuminosityFormat)),

		// Output is an 8-bit RGB framebuffer
		streaksPatternTex(c.resources.loadTexture2D("data/StreaksRotationPattern.bmp")),

		resultTex(c.resources.addColorTexture()),

		brightness(c, DescriptorSetInfo { .textures = { fsTextureAttachment(input) } }, { brightnessTex }, "data/shaders/chapter08/BrightPass.frag"),

		bloomX1(c, DescriptorSetInfo { .textures = { fsTextureAttachment(brightnessTex) } }, { bloomX1Tex }, "data/shaders/chapter08/BloomX.frag"),
		bloomY1(c, DescriptorSetInfo { .textures = { fsTextureAttachment(   bloomX1Tex) } }, { bloomY1Tex }, "data/shaders/chapter08/BloomY.frag"),
		bloomX2(c, DescriptorSetInfo { .textures = { fsTextureAttachment(   bloomY1Tex) } }, { bloomX2Tex }, "data/shaders/chapter08/BloomX.frag"),
		bloomY2(c, DescriptorSetInfo { .textures = { fsTextureAttachment(   bloomX2Tex) } }, { bloomY2Tex }, "data/shaders/chapter08/BloomY.frag"),

		streaks1(c, DescriptorSetInfo { .textures = { fsTextureAttachment(  bloomY2Tex), fsTextureAttachment(streaksPatternTex) } }, { streaks1Tex }, "data/shaders/chapter08/Streaks.frag"),
		streaks2(c, DescriptorSetInfo { .textures = { fsTextureAttachment( streaks1Tex), fsTextureAttachment(streaksPatternTex) } }, { streaks2Tex }, "data/shaders/chapter08/Streaks.frag"),

		composer(c, DescriptorSetInfo { .buffers = { uniformBufferAttachment(bloomUniforms, 0, 0, VK_SHADER_STAGE_FRAGMENT_BIT) }, .textures = { fsTextureAttachment(input), fsTextureAttachment(avgLuminance), fsTextureAttachment(streaks2Tex) } },
			{ resultTex }, "data/shaders/chapter08/BloomCompose.frag")
	{
		renderers_.emplace_back(brightness, false);
		renderers_.emplace_back(bloomX1, false);
		renderers_.emplace_back(bloomY1, false);
		renderers_.emplace_back(bloomX2, false);
		renderers_.emplace_back(bloomY2, false);

		renderers_.emplace_back(streaks1, false);
		renderers_.emplace_back(streaks2, false);

		renderers_.emplace_back(composer, false);
	}

	inline VulkanTexture getBloom1() const { return bloomY1Tex; }
	inline VulkanTexture getBloom2() const { return bloomY2Tex; }

	inline VulkanTexture getBrightness() const { return brightnessTex; }

	inline VulkanTexture getStreaks1() const { return streaks1Tex; }
	inline VulkanTexture getStreaks2() const { return streaks2Tex; }

	inline VulkanTexture getResult() const { return resultTex; }

	void updateBuffers(size_t currentImage) override
	{
		// all sub-renderers do not need buffer updates, so we override this method completely
		uploadBufferData(ctx_.vkDev, bloomUniforms.memory, 0, &ubo, sizeof(ubo));
	}

	inline void setParameters(float bloomStrength, float midGray, float white = 1.0f)
	{
		ubo.bloomStrength = bloomStrength;
		ubo.midGray       = midGray;
		ubo.white         = white;
	}

private:
	struct UniformBuffer
	{
		float bloomStrength = 0.3f;
		float midGray = 0.2f;
		float white = 1.0f;
	} ubo;

	VulkanBuffer bloomUniforms;

	// Static texture with rotation pattern
	VulkanTexture streaksPatternTex;

	// Texture with values above 1.0
	VulkanTexture brightnessTex;

	// First pass of blurring
	VulkanTexture bloomX1Tex;
	VulkanTexture bloomY1Tex;

	// Second pass of blurring
	VulkanTexture bloomX2Tex;
	VulkanTexture bloomY2Tex;

	VulkanTexture streaks1Tex;
	VulkanTexture streaks2Tex;

	// Composed Source + Brightness
	VulkanTexture resultTex;

	QuadProcessor brightness;

	QuadProcessor bloomX1;
	QuadProcessor bloomY1;
	QuadProcessor bloomX2;
	QuadProcessor bloomY2;

	QuadProcessor streaks1;
	QuadProcessor streaks2;

	QuadProcessor composer;
};
