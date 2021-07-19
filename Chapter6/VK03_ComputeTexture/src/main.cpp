#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkRenderers/VulkanSingleQuad.h"
#include "shared/vkRenderers/VulkanComputedImage.h"
#include "shared/vkRenderers/VulkanClear.h"
#include "shared/vkRenderers/VulkanFinish.h"

std::unique_ptr<VulkanClear> clear;
std::unique_ptr<VulkanSingleQuadRenderer> quad;
std::unique_ptr<VulkanFinish> finish;

std::unique_ptr<ComputedImage> imgGen;

VulkanInstance vk;
VulkanRenderDevice vkDev;

GLFWwindow * window;

const uint32_t kScreenWidth = 1280;
const uint32_t kScreenHeight = 720;

void composeFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	clear->fillCommandBuffer(commandBuffer, imageIndex);

	insertComputedImageBarrier(commandBuffer, imgGen->computed.image);

	quad->fillCommandBuffer(commandBuffer, imageIndex);
	finish->fillCommandBuffer(commandBuffer, imageIndex);
}

int main()
{
	window = initVulkanApp(kScreenWidth, kScreenHeight);

	glfwSetKeyCallback(
		window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			const bool pressed = action != GLFW_RELEASE;
			if (key == GLFW_KEY_ESCAPE && pressed)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		});

	createInstance(&vk.instance);

	if (!setupDebugCallbacks(vk.instance, &vk.messenger, &vk.reportCallback) ||
	    glfwCreateWindowSurface(vk.instance, window, nullptr, &vk.surface) ||
	    !initVulkanRenderDeviceWithCompute(vk, vkDev, kScreenWidth, kScreenHeight, VkPhysicalDeviceFeatures {}))
		exit(EXIT_FAILURE);

	VulkanImage nullTexture = { .image = VK_NULL_HANDLE, .imageView = VK_NULL_HANDLE };

	clear = std::make_unique<VulkanClear>(vkDev, nullTexture);
	finish = std::make_unique<VulkanFinish>(vkDev, nullTexture);
	imgGen = std::make_unique<ComputedImage>(vkDev, "data/shaders/chapter06/VK03_compute_texture.comp", 1024, 1024, false);
	quad = std::make_unique<VulkanSingleQuadRenderer>(vkDev, imgGen->computed, imgGen->computedImageSampler);

	do
	{
		float thisTime = (float)glfwGetTime();
		imgGen->fillComputeCommandBuffer(&thisTime, sizeof(float), imgGen->computedWidth / 16, imgGen->computedHeight / 16, 1);
		imgGen->submit();
		vkDeviceWaitIdle(vkDev.device);

		drawFrame(vkDev, [](uint32_t){}, composeFrame);

		glfwPollEvents();

		vkDeviceWaitIdle(vkDev.device);
	} while (!glfwWindowShouldClose(window));

	imgGen->waitFence();

	destroyVulkanImage(vkDev.device, imgGen->computed);
	vkDestroySampler(vkDev.device, imgGen->computedImageSampler, nullptr);

	quad = nullptr;
	clear = nullptr;
	finish = nullptr;
	imgGen = nullptr;

	destroyVulkanRenderDevice(vkDev);
	destroyVulkanInstance(vk);

	glfwTerminate();
	glslang_finalize_process();

	return 0;
}
