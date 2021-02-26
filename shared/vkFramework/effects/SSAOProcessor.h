#pragma once
#include "shared/vkFramework/CompositeRenderer.h"
#include "shared/vkFramework/VulkanShaderProcessor.h"

const int SSAOWidth = 0; // smaller SSAO buffer can be used 512
const int SSAOHeight = 0; // 512;

struct SSAOProcessor: public CompositeRenderer
{
	SSAOProcessor(VulkanRenderContext&ctx, VulkanTexture colorTex, VulkanTexture depthTex, VulkanTexture outputTex):
		CompositeRenderer(ctx),

		rotateTex(ctx.resources.loadTexture2D("data/rot_texture.bmp")),
		SSAOTex(ctx.resources.addColorTexture(SSAOWidth, SSAOHeight)),
		SSAOBlurXTex(ctx.resources.addColorTexture(SSAOWidth, SSAOHeight)),
		SSAOBlurYTex(ctx.resources.addColorTexture(SSAOWidth, SSAOHeight)),

		SSAO(ctx,  { .textures = { fsTextureAttachment(depthTex), fsTextureAttachment(rotateTex) } },
			{ SSAOTex }, "data/shaders/chapter08/SSAO.frag"),
		BlurX(ctx, { .textures = { fsTextureAttachment(SSAOTex) } },
			{ SSAOBlurXTex }, "data/shaders/chapter08/SSAOBlurX.frag"),
		BlurY(ctx, { .textures = { fsTextureAttachment(SSAOBlurXTex) } },
			{ SSAOBlurYTex }, "data/shaders/chapter08/SSAOBlurY.frag"),
		SSAOFinal(ctx, { .textures = { fsTextureAttachment(colorTex), fsTextureAttachment(SSAOBlurYTex) } },
			{ outputTex }, "data/shaders/chapter08/SSAOFinal.frag")
	{
		renderers_.emplace_back(SSAO, false);
		renderers_.emplace_back(BlurX, false);
		renderers_.emplace_back(BlurY, false);
		renderers_.emplace_back(SSAOFinal, false);
	}

	inline VulkanTexture getSSAO()   const { return SSAOTex; }
	inline VulkanTexture getBlurX()  const { return SSAOBlurXTex; }
	inline VulkanTexture getBlurY()  const { return SSAOBlurYTex; }

private:
	VulkanTexture rotateTex;
	VulkanTexture SSAOTex, SSAOBlurXTex, SSAOBlurYTex;

	QuadProcessor SSAO, BlurX, BlurY, SSAOFinal;
};
