#pragma once
#include "shared/vkFramework/CompositeRenderer.h"

const VkFormat LuminosityFormat = VK_FORMAT_R16G16B16A16_SFLOAT; /// VK_FORMAT_R32G32B32A32_SFLOAT

const int LuminosityWidth  = 64;
const int LuminosityHeight = 64;

struct LuminanceCalculator: public CompositeRenderer
{
	LuminanceCalculator(VulkanRenderContext& c): CompositeRenderer(c),
		lumDepth64(c.resources.addDepthTexture(LuminosityWidth, LuminosityHeight)),

		lumTex64(c.resources.addColorTexture(LuminosityWidth, LuminosityHeight, LuminosityFormat)),
		lumTex32(c.resources.addColorTexture(LuminosityWidth /  2, LuminosityHeight /  2, LuminosityFormat)),
		lumTex16(c.resources.addColorTexture(LuminosityWidth /  4, LuminosityHeight /  4, LuminosityFormat)),
		lumTex08(c.resources.addColorTexture(LuminosityWidth /  8, LuminosityHeight /  8, LuminosityFormat)),
		lumTex04(c.resources.addColorTexture(LuminosityWidth / 16, LuminosityHeight / 16, LuminosityFormat)),
		lumTex02(c.resources.addColorTexture(LuminosityWidth / 32, LuminosityHeight / 32, LuminosityFormat)),
		lumTex01(c.resources.addColorTexture(LuminosityWidth / 64, LuminosityHeight / 64, LuminosityFormat)),

		lum64_To_32(c, DescriptorSetInfo { .textures = { fsTextureAttachment(lumTex64) } }, { lumTex32 }, "data/shaders/chapter08/downscale2x2.frag"),
		lum32_To_16(c, DescriptorSetInfo { .textures = { fsTextureAttachment(lumTex32) } }, { lumTex16 }, "data/shaders/chapter08/downscale2x2.frag"),
		lum16_To_08(c, DescriptorSetInfo { .textures = { fsTextureAttachment(lumTex16) } }, { lumTex08 }, "data/shaders/chapter08/downscale2x2.frag"),
		lum08_To_04(c, DescriptorSetInfo { .textures = { fsTextureAttachment(lumTex08) } }, { lumTex04 }, "data/shaders/chapter08/downscale2x2.frag"),
		lum04_To_02(c, DescriptorSetInfo { .textures = { fsTextureAttachment(lumTex04) } }, { lumTex02 }, "data/shaders/chapter08/downscale2x2.frag"),
		lum02_To_01(c, DescriptorSetInfo { .textures = { fsTextureAttachment(lumTex02) } }, { lumTex01 }, "data/shaders/chapter08/downscale2x2.frag")
	{
		renderers_.emplace_back(lum64_To_32, false);
		renderers_.emplace_back(lum32_To_16, false);
		renderers_.emplace_back(lum16_To_08, false);
		renderers_.emplace_back(lum08_To_04, false);
		renderers_.emplace_back(lum04_To_02, false);
		renderers_.emplace_back(lum02_To_01, false);
	}

	inline VulkanTexture getDepth()    const { return lumDepth64; }
	inline VulkanTexture getInput()    const { return lumTex64; }
	inline VulkanTexture getResult32() const { return lumTex32; }
	inline VulkanTexture getResult16() const { return lumTex16; }
	inline VulkanTexture getResult08() const { return lumTex08; }
	inline VulkanTexture getResult04() const { return lumTex04; }
	inline VulkanTexture getResult02() const { return lumTex02; }
	inline VulkanTexture getResult01() const { return lumTex01; }

private:
	VulkanTexture lumDepth64;
	VulkanTexture lumTex64;
	VulkanTexture lumTex32;
	VulkanTexture lumTex16;
	VulkanTexture lumTex08;
	VulkanTexture lumTex04;
	VulkanTexture lumTex02;
	VulkanTexture lumTex01;

	QuadProcessor lum64_To_32;
	QuadProcessor lum32_To_16;
	QuadProcessor lum16_To_08;
	QuadProcessor lum08_To_04;
	QuadProcessor lum04_To_02;
	QuadProcessor lum02_To_01;
};
