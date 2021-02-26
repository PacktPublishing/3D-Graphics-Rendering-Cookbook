#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>

#include "shared/Utils.h"
#include "shared/UtilsVulkan.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;
using glm::vec3;
using glm::vec4;

const uint32_t kScreenWidth = 1280;
const uint32_t kScreenHeight = 720;

GLFWwindow* window;

struct UniformBuffer
{
	mat4 mvp;
};

static constexpr VkClearColorValue clearValueColor = { 1.0f, 1.0f, 1.0f, 1.0f };

size_t vertexBufferSize;
size_t indexBufferSize;

VulkanInstance vk;
VulkanRenderDevice vkDev;

struct VulkanState
{
	// 1. Descriptor set (layout + pool + sets) -> uses uniform buffers, textures, framebuffers
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	// 2. 
	std::vector<VkFramebuffer> swapchainFramebuffers;

	// 3. Pipeline & render pass (using DescriptorSets & pipeline state options)
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// 4. Uniform buffer
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	// 5. Storage Buffer with index and vertex data
	VkBuffer storageBuffer;
	VkDeviceMemory storageBufferMemory;

	// 6. Depth buffer
	VulkanImage depthTexture;

	VkSampler textureSampler;
	VulkanImage texture;
} vkState;

bool createDescriptorSet()
{
	const std::array<VkDescriptorSetLayoutBinding, 4> bindings = {
		descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		descriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	const VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device, &layoutInfo, nullptr, &vkState.descriptorSetLayout));

	std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(), vkState.descriptorSetLayout);

	const VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = vkState.descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),
		.pSetLayouts = layouts.data()
	};

	vkState.descriptorSets.resize(vkDev.swapchainImages.size());

	VK_CHECK(vkAllocateDescriptorSets(vkDev.device, &allocInfo, vkState.descriptorSets.data()));

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		const VkDescriptorBufferInfo bufferInfo = {
			.buffer = vkState.uniformBuffers[i],
			.offset = 0,
			.range = sizeof(UniformBuffer)
		};
		const VkDescriptorBufferInfo bufferInfo2 = {
			.buffer = vkState.storageBuffer,
			.offset = 0,
			.range = vertexBufferSize
		};
		const VkDescriptorBufferInfo bufferInfo3 = {
			.buffer = vkState.storageBuffer,
			.offset = vertexBufferSize,
			.range = indexBufferSize
		};
		const VkDescriptorImageInfo imageInfo = {
			.sampler = vkState.textureSampler,
			.imageView = vkState.texture.imageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		const std::array<VkWriteDescriptorSet, 4> descriptorWrites = {
			VkWriteDescriptorSet {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = vkState.descriptorSets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufferInfo
			},
			VkWriteDescriptorSet {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = vkState.descriptorSets[i],
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.pBufferInfo = &bufferInfo2
			},
			VkWriteDescriptorSet {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = vkState.descriptorSets[i],
				.dstBinding = 2,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.pBufferInfo = &bufferInfo3
			},
			VkWriteDescriptorSet {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = vkState.descriptorSets[i],
				.dstBinding = 3,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = &imageInfo
			},
		};

		vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return true;
}

bool fillCommandBuffers(size_t i)
{
	const VkCommandBufferBeginInfo bi =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		.pInheritanceInfo = nullptr
	};

	const std::array<VkClearValue, 2> clearValues =
	{
		VkClearValue { .color = clearValueColor },
		VkClearValue { .depthStencil = { 1.0f, 0 } }
	};

	const VkRect2D screenRect = {
		.offset = { 0, 0 },
		.extent = {.width = kScreenWidth, .height = kScreenHeight }
	};

	VK_CHECK(vkBeginCommandBuffer(vkDev.commandBuffers[i], &bi));

	const VkRenderPassBeginInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = vkState.renderPass,
		.framebuffer = vkState.swapchainFramebuffers[i],
		.renderArea = screenRect,
		.clearValueCount = static_cast<uint32_t>(clearValues.size()),
		.pClearValues = clearValues.data()
	};

	vkCmdBeginRenderPass(vkDev.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(vkDev.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkState.graphicsPipeline);

	vkCmdBindDescriptorSets(vkDev.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkState.pipelineLayout, 0, 1, &vkState.descriptorSets[i], 0, nullptr);
	vkCmdDraw(vkDev.commandBuffers[i], static_cast<uint32_t>(indexBufferSize / (sizeof(unsigned int))), 1, 0, 0);

	vkCmdEndRenderPass(vkDev.commandBuffers[i]);

	VK_CHECK(vkEndCommandBuffer(vkDev.commandBuffers[i]));

	return true;
}

void updateUniformBuffer(uint32_t currentImage, const void* uboData, size_t uboSize)
{
	void* data = nullptr;
	vkMapMemory(vkDev.device, vkState.uniformBuffersMemory[currentImage], 0, uboSize, 0, &data);
		memcpy(data, uboData, uboSize);
	vkUnmapMemory(vkDev.device, vkState.uniformBuffersMemory[currentImage]);
}

bool createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBuffer);

	vkState.uniformBuffers.resize(vkDev.swapchainImages.size());
	vkState.uniformBuffersMemory.resize(vkDev.swapchainImages.size());

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		if (!createBuffer(vkDev.device, vkDev.physicalDevice, bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vkState.uniformBuffers[i], vkState.uniformBuffersMemory[i]))
		{
			printf("Fail: buffers\n");
			return false;
		}
	}

	return true;
}

bool initVulkan()
{
	createInstance(&vk.instance);

	if (!setupDebugCallbacks(vk.instance, &vk.messenger, &vk.reportCallback))
		exit(EXIT_FAILURE);

	if (glfwCreateWindowSurface(vk.instance, window, nullptr, &vk.surface))
		exit(EXIT_FAILURE);

	if (!initVulkanRenderDevice(vk, vkDev, kScreenWidth, kScreenHeight, isDeviceSuitable, { .geometryShader = VK_TRUE }))
		exit(EXIT_FAILURE);

	if (!createTexturedVertexBuffer(vkDev, "data/rubber_duck/scene.gltf", &vkState.storageBuffer, &vkState.storageBufferMemory, &vertexBufferSize, &indexBufferSize) ||
		!createUniformBuffers())
	{
		printf("Cannot create data buffers\n"); fflush(stdout);
		exit(1);
	}

	createTextureImage(vkDev, "data/rubber_duck/textures/Duck_baseColor.png", vkState.texture.image, vkState.texture.imageMemory);
	createImageView(vkDev.device, vkState.texture.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &vkState.texture.imageView);
	createTextureSampler(vkDev.device, &vkState.textureSampler);

	createDepthResources(vkDev, kScreenWidth, kScreenHeight, vkState.depthTexture);

	if (!createDescriptorPool(vkDev, 1, 2, 1, &vkState.descriptorPool) ||
		!createDescriptorSet() ||
		!createColorAndDepthRenderPass(vkDev, true, &vkState.renderPass, RenderPassCreateInfo{ .clearColor_ = true, .clearDepth_ = true, .flags_ = eRenderPassBit_First|eRenderPassBit_Last }) ||
		!createPipelineLayout(vkDev.device, vkState.descriptorSetLayout, &vkState.pipelineLayout) ||
		!createGraphicsPipeline(vkDev, vkState.renderPass, vkState.pipelineLayout, { "data/shaders/chapter03/VK02.vert", "data/shaders/chapter03/VK02.frag", "data/shaders/chapter03/VK02.geom" }, &vkState.graphicsPipeline))
	{
		printf("Failed to create pipeline\n"); fflush(stdout);
		exit(0);
	}

	createColorAndDepthFramebuffers(vkDev, vkState.renderPass, vkState.depthTexture.imageView, vkState.swapchainFramebuffers);

	return VK_SUCCESS;
}

void terminateVulkan()
{
	vkDestroyBuffer(vkDev.device, vkState.storageBuffer, nullptr);
	vkFreeMemory(vkDev.device, vkState.storageBufferMemory, nullptr);

	for (size_t i = 0; i < vkDev.swapchainImages.size(); i++)
	{
		vkDestroyBuffer(vkDev.device, vkState.uniformBuffers[i], nullptr);
		vkFreeMemory(vkDev.device, vkState.uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorSetLayout(vkDev.device, vkState.descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(vkDev.device, vkState.descriptorPool, nullptr);

	for (auto framebuffer : vkState.swapchainFramebuffers)
	{
		vkDestroyFramebuffer(vkDev.device, framebuffer, nullptr);
	}

	vkDestroySampler(vkDev.device, vkState.textureSampler, nullptr);
	destroyVulkanImage(vkDev.device, vkState.texture);

	destroyVulkanImage(vkDev.device, vkState.depthTexture);

	vkDestroyRenderPass(vkDev.device, vkState.renderPass, nullptr);

	vkDestroyPipelineLayout(vkDev.device, vkState.pipelineLayout, nullptr);
	vkDestroyPipeline(vkDev.device, vkState.graphicsPipeline, nullptr);

	destroyVulkanRenderDevice(vkDev);

	destroyVulkanInstance(vk);
}

/* Common main() and drawOverlay() routines for VK04, VK05 and VK06 samples */

bool drawOverlay()
{
	uint32_t imageIndex = 0;
	if (vkAcquireNextImageKHR(vkDev.device, vkDev.swapchain, 0, vkDev.semaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
		return false;

	VK_CHECK(vkResetCommandPool(vkDev.device, vkDev.commandPool, 0));

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	const float ratio = width / (float)height;

	const mat4 m1 = glm::rotate(
		glm::translate(mat4(1.0f), vec3(0.f, 0.5f, -1.5f)) * glm::rotate(mat4(1.f), glm::pi<float>(),
		vec3(1, 0, 0)),
		(float)glfwGetTime(),
		vec3(0.0f, 1.0f, 0.0f)
	);
	const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);

	const UniformBuffer ubo{ .mvp = p * m1 };

	updateUniformBuffer(imageIndex, &ubo, sizeof(ubo));

	fillCommandBuffers(imageIndex);

	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // or even VERTEX_SHADER_STAGE

	const VkSubmitInfo si =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &vkDev.semaphore,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &vkDev.commandBuffers[imageIndex],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &vkDev.renderSemaphore
	};

	VK_CHECK(vkQueueSubmit(vkDev.graphicsQueue, 1, &si, nullptr));

	const VkPresentInfoKHR pi =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &vkDev.renderSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &vkDev.swapchain,
		.pImageIndices = &imageIndex
	};

	VK_CHECK(vkQueuePresentKHR(vkDev.graphicsQueue, &pi));
	VK_CHECK(vkDeviceWaitIdle(vkDev.device));

	return true;
}

int main()
{
	glslang_initialize_process();

	volkInitialize();

	if (!glfwInit())
		exit(EXIT_FAILURE);

	if (!glfwVulkanSupported())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "VulkanApp", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(
		window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	);

	initVulkan();

	while (!glfwWindowShouldClose(window))
	{
		drawOverlay();
		glfwPollEvents();
	}

	terminateVulkan();
	glfwTerminate();
	glslang_finalize_process();

	return 0;
}
