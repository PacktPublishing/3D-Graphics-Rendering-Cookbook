#pragma once
#include "shared/vkFramework/effects/LuminanceCalculator.h"

#include "shared/vkFramework/Barriers.h"

struct HDRUniformBuffer
{
	float exposure;
	float maxWhite;
	float bloomStrength;
	float adaptationSpeed;
};

/** Apply bloom to input buffer */
struct HDRProcessor: public CompositeRenderer
{
	HDRProcessor(VulkanRenderContext& c, VulkanTexture input, VulkanTexture avgLuminance, BufferAttachment uniformBuffer): CompositeRenderer(c),

		brightnessTex(c.resources.addColorTexture(0, 0, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),

		adaptedLuminanceTex1(c.resources.addColorTexture(1, 1, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),
		adaptedLuminanceTex2(c.resources.addColorTexture(1, 1, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),

		bloomX1Tex(c.resources.addColorTexture(0, 0, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),
		bloomY1Tex(c.resources.addColorTexture(0, 0, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),
		bloomX2Tex(c.resources.addColorTexture(0, 0, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),
		bloomY2Tex(c.resources.addColorTexture(0, 0, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),

		streaks1Tex(c.resources.addColorTexture(0, 0, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),
		streaks2Tex(c.resources.addColorTexture(0, 0, LuminosityFormat, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)),

		// Output is an 8-bit RGB framebuffer
		streaksPatternTex(c.resources.loadTexture2D("data/StreaksRotationPattern.bmp")),

		resultTex(c.resources.addColorTexture()),

		brightness(c, DescriptorSetInfo { .textures = { fsTextureAttachment(input) } }, { brightnessTex }, "data/shaders/chapter08/VK03_BrightPass.frag"),

		bloomX1(c, DescriptorSetInfo { .textures = { fsTextureAttachment(brightnessTex) } }, { bloomX1Tex }, "data/shaders/chapter08/VK03_BloomX.frag"),
		bloomY1(c, DescriptorSetInfo { .textures = { fsTextureAttachment(   bloomX1Tex) } }, { bloomY1Tex }, "data/shaders/chapter08/VK03_BloomY.frag"),
		bloomX2(c, DescriptorSetInfo { .textures = { fsTextureAttachment(   bloomY1Tex) } }, { bloomX2Tex }, "data/shaders/chapter08/VK03_BloomX.frag"),
		bloomY2(c, DescriptorSetInfo { .textures = { fsTextureAttachment(   bloomX2Tex) } }, { bloomY2Tex }, "data/shaders/chapter08/VK03_BloomY.frag"),

		streaks1(c, DescriptorSetInfo { .textures = { fsTextureAttachment(  bloomY2Tex), fsTextureAttachment(streaksPatternTex) } }, { streaks1Tex }, "data/shaders/chapter08/VK03_Streaks.frag"),
		streaks2(c, DescriptorSetInfo { .textures = { fsTextureAttachment( streaks1Tex), fsTextureAttachment(streaksPatternTex) } }, { streaks2Tex }, "data/shaders/chapter08/VK03_Streaks.frag"),

		adaptationEven(c, DescriptorSetInfo { .buffers = { uniformBuffer }, .textures = { fsTextureAttachment(avgLuminance), fsTextureAttachment(adaptedLuminanceTex1) } },
			{ adaptedLuminanceTex2 }, "data/shaders/chapter08/VK03_LightAdaptation.frag"),
		adaptationOdd(c, DescriptorSetInfo { .buffers = { uniformBuffer }, .textures = { fsTextureAttachment(avgLuminance), fsTextureAttachment(adaptedLuminanceTex2) } },
			{ adaptedLuminanceTex1 }, "data/shaders/chapter08/VK03_LightAdaptation.frag"),

		adaptation1ToColor(c, adaptedLuminanceTex1),
		adaptation1ToShader(c, adaptedLuminanceTex1),

		adaptation2ToColor(c, adaptedLuminanceTex2),
		adaptation2ToShader(c, adaptedLuminanceTex2),

		composerEven(c, DescriptorSetInfo { .buffers = { uniformBuffer }, .textures = { fsTextureAttachment(input), fsTextureAttachment(adaptedLuminanceTex2), fsTextureAttachment(streaks2Tex) } },
			{ resultTex }, "data/shaders/chapter08/VK03_HDR.frag"),
		composerOdd (c, DescriptorSetInfo { .buffers = { uniformBuffer }, .textures = { fsTextureAttachment(input), fsTextureAttachment(adaptedLuminanceTex1), fsTextureAttachment(streaks2Tex) } },
			{ resultTex }, "data/shaders/chapter08/VK03_HDR.frag"),

		bloomX1ToColor(c, bloomX1Tex),
		bloomX1ToShader(c, bloomX1Tex),
		bloomX2ToColor(c, bloomX2Tex),
		bloomX2ToShader(c, bloomX2Tex),

		bloomY1ToColor(c, bloomY1Tex),
		bloomY1ToShader(c, bloomY1Tex),
		bloomY2ToColor(c, bloomY2Tex),
		bloomY2ToShader(c, bloomY2Tex),

		streaks1ToColor(c, streaks1Tex),
		streaks1ToShader(c, streaks1Tex),
		streaks2ToColor(c, streaks2Tex),
		streaks2ToShader(c, streaks2Tex),

		brightnessToColor(c, brightnessTex),
		brightnessToShader(c, brightnessTex),

		resultToColor(c, resultTex),
		resultToShader(c, resultTex)
	{
		renderers_.emplace_back(brightnessToColor, false);
		renderers_.emplace_back(brightness, false);
		renderers_.emplace_back(brightnessToShader, false);

		renderers_.emplace_back(bloomX1ToColor, false);
		renderers_.emplace_back(bloomX1, false);
		renderers_.emplace_back(bloomX1ToShader, false);

		renderers_.emplace_back(bloomY1ToColor, false);
		renderers_.emplace_back(bloomY1, false);
		renderers_.emplace_back(bloomY1ToShader, false);

		renderers_.emplace_back(bloomX2ToColor, false);
		renderers_.emplace_back(bloomX2, false);
		renderers_.emplace_back(bloomX2ToShader, false);

		renderers_.emplace_back(bloomY2ToColor, false);
		renderers_.emplace_back(bloomY2, false);
		renderers_.emplace_back(bloomY2ToShader, false);

		renderers_.emplace_back(streaks1ToColor, false);
		renderers_.emplace_back(streaks1, false);
		renderers_.emplace_back(streaks1ToShader, false);

		renderers_.emplace_back(streaks2ToColor, false);
		renderers_.emplace_back(streaks2, false);
		renderers_.emplace_back(streaks2ToShader, false);

		renderers_.emplace_back(adaptation2ToColor, false);  // 21
		renderers_.emplace_back(adaptationEven, false);      // 22
		renderers_.emplace_back(adaptation2ToShader, false); // 23

		renderers_.emplace_back(adaptation1ToColor, false);  // 24
		renderers_.emplace_back(adaptationOdd, false);       // 25
		renderers_.emplace_back(adaptation1ToShader, false); // 26

		renderers_[21].enabled_ = false; // disable adaptationProcessor at the beginning
		renderers_[22].enabled_ = false;
		renderers_[23].enabled_ = false;

		renderers_.emplace_back(resultToColor, false); // 27
		renderers_.emplace_back(composerEven, false);  // 28
		renderers_.emplace_back(composerOdd, false);   // 29
		renderers_[29].enabled_ = false; // disable composerOdd at the beginning

		renderers_.emplace_back(resultToShader, false);

		// Convert 32.0 to S5.10 fixed point format (half-float) manually for RGB channels, Set alpha to 1.0
//		const uint16_t brightPixel[4] = { 0x5400, 0x5400, 0x5400, 0x3C00 }; // 64.0 as initial value
		const uint16_t brightPixel[4] = { 0x5000, 0x5000, 0x5000, 0x3C00 }; // 32.0 as initial value
		updateTextureImage(c.vkDev, adaptedLuminanceTex1.image.image, adaptedLuminanceTex1.image.imageMemory, 1, 1, LuminosityFormat, 1, &brightPixel, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		updateTextureImage(c.vkDev, adaptedLuminanceTex2.image.image, adaptedLuminanceTex2.image.imageMemory, 1, 1, LuminosityFormat, 1, &brightPixel, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void fillCommandBuffer(VkCommandBuffer cmdBuffer, size_t currentImage, VkFramebuffer fb1 = VK_NULL_HANDLE, VkRenderPass rp1 = VK_NULL_HANDLE) override
	{
		// Call base method
		CompositeRenderer::fillCommandBuffer(cmdBuffer, currentImage, fb1, rp1);
		// Swap avgLuminance inputs for adaptation and composer
		static const std::vector<int> switchIndices { 21, 22, 23, 24, 25, 26, 28, 29 };
		for (auto i: switchIndices)
			renderers_[i].enabled_ = !renderers_[i].enabled_;
	}

	inline VulkanTexture getBloom1() const { return bloomY1Tex; }
	inline VulkanTexture getBloom2() const { return bloomY2Tex; }

	inline VulkanTexture getBrightness() const { return brightnessTex; }

	inline VulkanTexture getStreaks1() const { return streaks1Tex; }
	inline VulkanTexture getStreaks2() const { return streaks2Tex; }

	inline VulkanTexture getAdaptatedLum1() const { return adaptedLuminanceTex1; }
	inline VulkanTexture getAdaptatedLum2() const { return adaptedLuminanceTex2; }

	inline VulkanTexture getResult() const { return resultTex; }

private:
	// Static texture with rotation pattern
	VulkanTexture streaksPatternTex;

	// Texture with values above 1.0
	VulkanTexture brightnessTex;

	// The ping-pong texture pair for adapted luminances
	VulkanTexture adaptedLuminanceTex1, adaptedLuminanceTex2;

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

	// Light Adaptation processing
	QuadProcessor adaptationEven;
	QuadProcessor adaptationOdd;

	ShaderOptimalToColorBarrier adaptation1ToColor;
	ColorToShaderOptimalBarrier adaptation1ToShader;

	ShaderOptimalToColorBarrier adaptation2ToColor;
	ColorToShaderOptimalBarrier adaptation2ToShader;

	// Final composition
	QuadProcessor composerEven;
	QuadProcessor composerOdd;

	// barriers
	ShaderOptimalToColorBarrier bloomX1ToColor;
	ColorToShaderOptimalBarrier bloomX1ToShader;
	ShaderOptimalToColorBarrier bloomX2ToColor;
	ColorToShaderOptimalBarrier bloomX2ToShader;

	ShaderOptimalToColorBarrier bloomY1ToColor;
	ColorToShaderOptimalBarrier bloomY1ToShader;
	ShaderOptimalToColorBarrier bloomY2ToColor;
	ColorToShaderOptimalBarrier bloomY2ToShader;

	ShaderOptimalToColorBarrier streaks1ToColor;
	ColorToShaderOptimalBarrier streaks1ToShader;
	ShaderOptimalToColorBarrier streaks2ToColor;
	ColorToShaderOptimalBarrier streaks2ToShader;

	ShaderOptimalToColorBarrier brightnessToColor;
	ColorToShaderOptimalBarrier brightnessToShader;

	ShaderOptimalToColorBarrier resultToColor;
	ColorToShaderOptimalBarrier resultToShader;
};
