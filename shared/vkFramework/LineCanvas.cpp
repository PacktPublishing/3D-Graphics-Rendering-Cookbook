#include "shared/vkFramework/LineCanvas.h"

LineCanvas::LineCanvas(VulkanRenderContext& ctx,
                bool useDepth,
		const std::vector<VulkanTexture>& outputs,
		VkFramebuffer framebuffer,
		RenderPass screenRenderPass):
	Renderer(ctx)
{
	framebuffer_ = framebuffer;
	const PipelineInfo pInfo = initRenderPass(
		PipelineInfo {
			.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			.useDepth = useDepth,
			.useBlending = false
		}, outputs, screenRenderPass, ctx.screenRenderPass);

	const size_t imgCount = ctx.vkDev.swapchainImages.size();

	descriptorSets_.resize(imgCount);
	storages_.resize(imgCount);
	uniforms_.resize(imgCount);

	DescriptorSetInfo dsInfo = {
		.buffers = {
			uniformBufferAttachment(VulkanBuffer {}, 0, sizeof(UniformBuffer), VK_SHADER_STAGE_VERTEX_BIT),
			storageBufferAttachment(VulkanBuffer {}, 0, kMaxLinesDataSize,     VK_SHADER_STAGE_VERTEX_BIT)
		}
	};

	descriptorSetLayout_ = ctx.resources.addDescriptorSetLayout(dsInfo);
	descriptorPool_ = ctx.resources.addDescriptorPool(dsInfo, (uint32_t)imgCount);

	for(size_t i = 0 ; i < imgCount ; i++)
	{
		uniforms_[i] = ctx.resources.addUniformBuffer(sizeof(UniformBuffer));
		storages_[i] = ctx.resources.addStorageBuffer(kMaxLinesDataSize);

		dsInfo.buffers[0].buffer = uniforms_[i];
		dsInfo.buffers[1].buffer = storages_[i];

		descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
		ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
	}

	initPipeline({ "data/shaders/chapter04/Lines.vert", "data/shaders/chapter04/Lines.frag" }, pInfo);
}

void LineCanvas::updateBuffers(size_t currentImage)
{
	if (lines_.empty())
		return;

	const VkDeviceSize bufferSize = lines_.size() * sizeof(VertexData);

	uploadBufferData(ctx_.vkDev, storages_[currentImage].memory, 0, lines_.data(), bufferSize);

	const UniformBuffer ubo = {
		.mvp = mvp_,
		.time = (float)glfwGetTime()
	};

	updateUniformBuffer(currentImage, 0, sizeof(UniformBuffer), &ubo);
}

void LineCanvas::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb, VkRenderPass rp)
{
	if (lines_.empty())
		return;

	beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);

	vkCmdDraw( commandBuffer, static_cast<uint32_t>(lines_.size()), 1, 0, 0 );
	vkCmdEndRenderPass( commandBuffer );
}

void LineCanvas::line(const vec3& p1, const vec3& p2, const vec4& c)
{
	lines_.push_back( { .position = p1, .color = c } );
	lines_.push_back( { .position = p2, .color = c } );
}

void LineCanvas::plane3d(const vec3& o, const vec3& v1, const vec3& v2, int n1, int n2, float s1, float s2, const vec4& color, const vec4& outlineColor)
{
	line(o - s1 / 2.0f * v1 - s2 / 2.0f * v2, o - s1 / 2.0f * v1 + s2 / 2.0f * v2, outlineColor);
	line(o + s1 / 2.0f * v1 - s2 / 2.0f * v2, o + s1 / 2.0f * v1 + s2 / 2.0f * v2, outlineColor);

	line(o - s1 / 2.0f * v1 + s2 / 2.0f * v2, o + s1 / 2.0f * v1 + s2 / 2.0f * v2, outlineColor);
	line(o - s1 / 2.0f * v1 - s2 / 2.0f * v2, o + s1 / 2.0f * v1 - s2 / 2.0f * v2, outlineColor);

	for (int i = 1; i < n1; i++)
	{
		float t = ((float)i - (float)n1 / 2.0f) * s1 / (float)n1;
		const vec3 o1 = o + t * v1;
		line(o1 - s2 / 2.0f * v2, o1 + s2 / 2.0f * v2, color);
	}

	for (int i = 1; i < n2; i++)
	{
		const float t = ((float)i - (float)n2 / 2.0f) * s2 / (float)n2;
		const vec3 o2 = o + t * v2;
		line(o2 - s1 / 2.0f * v1, o2 + s1 / 2.0f * v1, color);
	}
}

static void drawBox3d_internal(LineCanvas& canvas, const glm::mat4& m, const glm::vec3& size, const glm::vec4& c)
{
	std::array<vec3, 8> pts = {
		vec3(+size.x, +size.y, +size.z),
		vec3(+size.x, +size.y, -size.z),
		vec3(+size.x, -size.y, +size.z),
		vec3(+size.x, -size.y, -size.z),
		vec3(-size.x, +size.y, +size.z),
		vec3(-size.x, +size.y, -size.z),
		vec3(-size.x, -size.y, +size.z),
		vec3(-size.x, -size.y, -size.z),
	};

	for (auto& p: pts)
		p = vec3(m * vec4(p, 1.f));

	canvas.line(pts[0], pts[1], c);
	canvas.line(pts[2], pts[3], c);
	canvas.line(pts[4], pts[5], c);
	canvas.line(pts[6], pts[7], c);

	canvas.line(pts[0], pts[2], c);
	canvas.line(pts[1], pts[3], c);
	canvas.line(pts[4], pts[6], c);
	canvas.line(pts[5], pts[7], c);

	canvas.line(pts[0], pts[4], c);
	canvas.line(pts[1], pts[5], c);
	canvas.line(pts[2], pts[6], c);
	canvas.line(pts[3], pts[7], c);
}

void drawBox3d(LineCanvas& canvas, const glm::mat4& m, const BoundingBox& box, const glm::vec4& color)
{
	drawBox3d_internal(canvas, m * glm::translate(glm::mat4(1.f), .5f * (box.min_ + box.max_)), 0.5f * vec3(box.max_ - box.min_), color);
}

void renderCameraFrustum(LineCanvas& C, const mat4& camView, const mat4& camProj, const vec4& camColor)
{
	const vec3 corners[] = {
		vec3(+1, -1, -1), vec3(+1, -1, +1),
		vec3(+1, +1, -1), vec3(+1, +1, +1),
		vec3(-1, +1, -1), vec3(-1, +1, +1),
		vec3(-1, -1, -1), vec3(-1, -1, +1)
	};
	vec3 pp[8];

	for(int i = 0 ; i < 8 ; i++) {
		glm::vec4 q = glm::inverse(camView) * glm::inverse(camProj) * glm::vec4(corners[i], 1.0f);
		pp[i] = glm::vec3(q.x / q.w, q.y / q.w, q.z / q.w);
	}

	for(int i = 0 ; i < 4 ; i++)
	{
		C.line(pp[i * 2 + 0], pp[i * 2 + 1], camColor);

		for(int k = 0 ; k < 2 ; k++)
			C.line(pp[k + i * 2], pp[k + ((i + 1) % 4) * 2], camColor);
	}
}
