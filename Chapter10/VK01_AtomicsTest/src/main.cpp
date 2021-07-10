#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkFramework/LineCanvas.h"
#include "shared/vkFramework/GuiRenderer.h"
#include "shared/vkFramework/QuadRenderer.h"
#include "shared/vkFramework/VulkanShaderProcessor.h"

struct node
{
	uint32_t idx;
	float xx, yy;
};

static_assert(sizeof(node) == 3 * sizeof(uint32_t));

float g_Percentage = 0.5f;

struct AtomicRenderer: public Renderer
{
	AtomicRenderer(VulkanRenderContext& ctx, VulkanBuffer sizeBuffer): Renderer(ctx)
	{
		const PipelineInfo pInfo = initRenderPass(PipelineInfo{}, { ctx.resources.addColorTexture() }, RenderPass(), ctx.screenRenderPass_NoDepth);

		uint32_t W = ctx.vkDev.framebufferWidth;
		uint32_t H = ctx.vkDev.framebufferHeight;

		const size_t imgCount = ctx.vkDev.swapchainImages.size();
		descriptorSets_.resize(imgCount);
		atomics_.resize(imgCount);
		output_.resize(imgCount);

		DescriptorSetInfo dsInfo = {
			.buffers = {
				storageBufferAttachment(VulkanBuffer {}, 0,     sizeof(uint32_t), VK_SHADER_STAGE_FRAGMENT_BIT),
				storageBufferAttachment(VulkanBuffer {}, 0, W * H * sizeof(node), VK_SHADER_STAGE_FRAGMENT_BIT),
				uniformBufferAttachment(sizeBuffer,      0,                    8, VK_SHADER_STAGE_FRAGMENT_BIT)
			}
		};

		descriptorSetLayout_ = ctx.resources.addDescriptorSetLayout(dsInfo);
		descriptorPool_ = ctx.resources.addDescriptorPool(dsInfo, (uint32_t)imgCount);

		for (size_t i = 0; i < imgCount; i++)
		{
			atomics_[i] = ctx.resources.addStorageBuffer(sizeof(uint32_t));
			output_[i] = ctx.resources.addStorageBuffer(W * H * sizeof(node));
			dsInfo.buffers[0].buffer = atomics_[i];
			dsInfo.buffers[1].buffer = output_[i];

			descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
			ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
		}

		initPipeline({ "data/shaders/chapter10/VK01_AtomicTest.vert", "data/shaders/chapter10/VK01_AtomicTest.frag" }, pInfo);
	}

	void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override
	{
		beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);
		vkCmdDraw(commandBuffer, 6, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffer);
	}

	void updateBuffers(size_t currentImage) override {
		uint32_t zeroCount = 0;
		uploadBufferData(ctx_.vkDev, atomics_[currentImage].memory, 0, &zeroCount, sizeof(uint32_t));
	}

	std::vector<VulkanBuffer>& getOutputs() { return output_; }

private:
	std::vector<VulkanBuffer> atomics_;
	std::vector<VulkanBuffer> output_;
};

struct AnimRenderer : public Renderer
{
	AnimRenderer(VulkanRenderContext& ctx, std::vector<VulkanBuffer>& pointBuffers, VulkanBuffer sizeBuffer) :
		Renderer(ctx),
		pointBuffers_(pointBuffers)
	{
		initRenderPass(PipelineInfo{}, {}, RenderPass(), ctx.screenRenderPass_NoDepth);

		const size_t imgCount = ctx.vkDev.swapchainImages.size();
		descriptorSets_.resize(imgCount);

		uint32_t W = ctx.vkDev.framebufferWidth;
		uint32_t H = ctx.vkDev.framebufferHeight;

		DescriptorSetInfo dsInfo = {
			.buffers = {
				storageBufferAttachment(VulkanBuffer {}, 0, W * H * sizeof(node), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
				uniformBufferAttachment(sizeBuffer,      0,                    8, VK_SHADER_STAGE_VERTEX_BIT)
			}
		};

		descriptorSetLayout_ = ctx.resources.addDescriptorSetLayout(dsInfo);
		descriptorPool_ = ctx.resources.addDescriptorPool(dsInfo, (uint32_t)imgCount);

		for (size_t i = 0; i < imgCount; i++)
		{
			dsInfo.buffers[0].buffer = pointBuffers_[i];
			descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
			ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
		}

		initPipeline({ "data/shaders/chapter10/VK01_AtomicVisualize.vert", "data/shaders/chapter10/VK01_AtomicVisualize.frag" }, PipelineInfo{ .topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST });
	}

	void fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb = VK_NULL_HANDLE, VkRenderPass rp = VK_NULL_HANDLE) override
	{
		uint32_t pointCount =
			uint32_t(ctx_.vkDev.framebufferWidth * ctx_.vkDev.framebufferHeight * g_Percentage);

		if (pointCount == 0)
			return;

		beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);
		vkCmdDraw(commandBuffer, pointCount, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffer);
	}

private:
	std::vector<VulkanBuffer>& pointBuffers_;
};

struct MyApp: public CameraApp
{
	MyApp()
	: CameraApp(-80, -80, {	.vertexPipelineStoresAndAtomics_ = true, .fragmentStoresAndAtomics_ = true })
	, sizeBuffer(ctx_.resources.addUniformBuffer(8))
	, atom(ctx_, sizeBuffer)
	, anim(ctx_, atom.getOutputs(), sizeBuffer)
	, imgui(ctx_, std::vector<VulkanTexture>{})
	{
		onScreenRenderers_.emplace_back(atom, false);
		onScreenRenderers_.emplace_back(anim, false);
		onScreenRenderers_.emplace_back(imgui, false);

		struct WH {
			float w, h;
		} wh {
			(float)ctx_.vkDev.framebufferWidth,
			(float)ctx_.vkDev.framebufferHeight
		};

		uploadBufferData(ctx_.vkDev, sizeBuffer.memory, 0, &wh, sizeof(wh));
	}

	void draw3D() override {}

	void drawUI() override {
		ImGui::Begin("Settings", nullptr);
		ImGui::SliderFloat("Percentage", &g_Percentage, 0.0f, 1.0f);
		ImGui::End();
	}

private:
	VulkanBuffer sizeBuffer;

	AtomicRenderer atom;
	AnimRenderer anim;
	GuiRenderer imgui;
};

int main()
{
	MyApp app;
	app.mainLoop();
	return 0;
}
