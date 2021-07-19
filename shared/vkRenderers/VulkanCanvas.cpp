#include "shared/vkRenderers/VulkanCanvas.h"
#include "shared/EasyProfilerWrapper.h"

VulkanCanvas::VulkanCanvas(VulkanRenderDevice& vkDev, VulkanImage depth)
: RendererBase(vkDev, depth)
{
	const size_t imgCount = vkDev.swapchainImages.size();

	storageBuffer_.resize(imgCount);
	storageBufferMemory_.resize(imgCount);

	for(size_t i = 0 ; i < imgCount ; i++)
	{
		if (!createBuffer(vkDev.device, vkDev.physicalDevice, kMaxLinesDataSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			storageBuffer_[i], storageBufferMemory_[i]))
		{
			printf("VaulkanCanvas: createBuffer() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	if (!createColorAndDepthRenderPass(vkDev, (depth.image != VK_NULL_HANDLE), &renderPass_, RenderPassCreateInfo()) ||
		!createUniformBuffers(vkDev, sizeof(UniformBuffer)) ||
		!createColorAndDepthFramebuffers(vkDev, renderPass_, depth.imageView, swapchainFramebuffers_) ||
		!createDescriptorPool(vkDev, 1, 1, 0, &descriptorPool_) ||
		!createDescriptorSet(vkDev) ||
		!createPipelineLayout(vkDev.device, descriptorSetLayout_, &pipelineLayout_) ||
		!createGraphicsPipeline(vkDev, renderPass_, pipelineLayout_, { "data/shaders/chapter04/Lines.vert", "data/shaders/chapter04/Lines.frag" }, &graphicsPipeline_, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, (depth.image != VK_NULL_HANDLE), true ))
	{
		printf("VulkanCanvas: failed to create pipeline\n");
		exit(EXIT_FAILURE);
	}
}

VulkanCanvas::~VulkanCanvas()
{
	for (size_t i = 0; i < swapchainFramebuffers_.size(); i++)
	{
		vkDestroyBuffer(device_, storageBuffer_[i], nullptr);
		vkFreeMemory(device_, storageBufferMemory_[i], nullptr);
	}
}

void VulkanCanvas::updateBuffer(VulkanRenderDevice& vkDev, size_t currentImage)
{
	if (lines_.empty())
		return;

	const VkDeviceSize bufferSize = lines_.size() * sizeof(VertexData);

	uploadBufferData(vkDev, storageBufferMemory_[currentImage], 0, lines_.data(), bufferSize);
}

bool VulkanCanvas::createDescriptorSet(VulkanRenderDevice& vkDev)
{
	const std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
	};

	const VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device, &layoutInfo, nullptr, &descriptorSetLayout_));

	std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(), descriptorSetLayout_);

	const VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool_,
		.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),
		.pSetLayouts = layouts.data()
	};

	descriptorSets_.resize(vkDev.swapchainImages.size());

	VK_CHECK(vkAllocateDescriptorSets(vkDev.device, &allocInfo, descriptorSets_.data()));

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		VkDescriptorSet ds = descriptorSets_[i];

		const VkDescriptorBufferInfo bufferInfo  = { uniformBuffers_[i], 0, sizeof(UniformBuffer) };
		const VkDescriptorBufferInfo bufferInfo2 = { storageBuffer_[i], 0, kMaxLinesDataSize };

		const std::array<VkWriteDescriptorSet, 2> descriptorWrites = {
			bufferWriteDescriptorSet(ds, &bufferInfo,	0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
			bufferWriteDescriptorSet(ds, &bufferInfo2, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
		};

		vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return true;
}

void VulkanCanvas::updateUniformBuffer(VulkanRenderDevice& vkDev, const glm::mat4& modelViewProj, float time, uint32_t currentImage)
{
	const UniformBuffer ubo = {
		.mvp = modelViewProj,
		.time = time
	};

	uploadBufferData(vkDev, uniformBuffersMemory_[currentImage], 0, &ubo, sizeof(ubo));
}

void VulkanCanvas::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage)
{
	EASY_FUNCTION();

	if (lines_.empty())
		return;

	beginRenderPass(commandBuffer, currentImage);

	vkCmdDraw( commandBuffer, static_cast<uint32_t>(lines_.size()), 1, 0, 0 );
	vkCmdEndRenderPass( commandBuffer );
}

void VulkanCanvas::clear()
{
	lines_.clear();
}

void VulkanCanvas::line(const vec3& p1, const vec3& p2, const vec4& c)
{
	lines_.push_back( { .position = p1, .color = c } );
	lines_.push_back( { .position = p2, .color = c } );
}

void VulkanCanvas::plane3d(const vec3& o, const vec3& v1, const vec3& v2, int n1, int n2, float s1, float s2, const vec4& color, const vec4& outlineColor)
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
