#include "VulkanApp.h"

#include "shared/vkFramework/Renderer.h"

Resolution detectResolution(int width, int height)
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const int code = glfwGetError(nullptr);

	if (code != 0)
	{
		printf("Monitor: %p; error = %x / %d\n", monitor, code, code);
		exit(255);
	}

	const GLFWvidmode* info = glfwGetVideoMode(monitor);

	const uint32_t windowW = width  > 0 ? width : (uint32_t)(info->width * width / -100);
	const uint32_t windowH = height > 0 ? height : (uint32_t)(info->height * height / -100);

	return Resolution{ .width = windowW, .height = windowH };
}

GLFWwindow* initVulkanApp(int width, int height, Resolution* resolution)
{
	glslang_initialize_process();

	volkInitialize();

	if (!glfwInit())
		exit(EXIT_FAILURE);

	if (!glfwVulkanSupported())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if (resolution)
	{
		*resolution = detectResolution(width, height);
		width  = resolution->width;
		height = resolution->height;
	}

	GLFWwindow* result = glfwCreateWindow(width, height, "VulkanApp", nullptr, nullptr);
	if (!result)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	return result;
}

bool drawFrame(VulkanRenderDevice& vkDev, const std::function<void(uint32_t)>& updateBuffersFunc, const std::function<void(VkCommandBuffer, uint32_t)>& composeFrameFunc)
{
	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(vkDev.device, vkDev.swapchain, 0, vkDev.semaphore, VK_NULL_HANDLE, &imageIndex);
	VK_CHECK(vkResetCommandPool(vkDev.device, vkDev.commandPool, 0));

	if (result != VK_SUCCESS) return false;

	updateBuffersFunc(imageIndex);

	VkCommandBuffer commandBuffer = vkDev.commandBuffers[imageIndex];

	const VkCommandBufferBeginInfo bi =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		.pInheritanceInfo = nullptr
	};

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &bi));

	composeFrameFunc(commandBuffer, imageIndex);

	VK_CHECK(vkEndCommandBuffer(commandBuffer));

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

void VulkanRenderContext::updateBuffers(uint32_t imageIndex)
{
	for (auto& r : onScreenRenderers_)
		if (r.enabled_)
			r.renderer_.updateBuffers(imageIndex);
}

void VulkanRenderContext::composeFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	const VkRect2D defaultScreenRect {
		.offset = { 0, 0 },
		.extent = {.width = vkDev.framebufferWidth, .height = vkDev.framebufferHeight }
	};

	static const VkClearValue defaultClearValues[2] =
	{
		VkClearValue { .color = { 1.0f, 1.0f, 1.0f, 1.0f } },
		VkClearValue { .depthStencil = { 1.0f, 0 } }
	};

	beginRenderPass(commandBuffer, clearRenderPass.handle, imageIndex, defaultScreenRect, VK_NULL_HANDLE, 2u, defaultClearValues);
	vkCmdEndRenderPass( commandBuffer );

	for (auto& r : onScreenRenderers_)
		if (r.enabled_)
		{
			RenderPass rp = r.useDepth_ ? screenRenderPass : screenRenderPass_NoDepth;
			VkFramebuffer fb = (r.useDepth_ ? swapchainFramebuffers : swapchainFramebuffers_NoDepth)[imageIndex];

			if (r.renderer_.renderPass_.handle != VK_NULL_HANDLE)
				rp = r.renderer_.renderPass_;
			if (r.renderer_.framebuffer_ != VK_NULL_HANDLE)
				fb = r.renderer_.framebuffer_;

			r.renderer_.fillCommandBuffer(commandBuffer, imageIndex, fb, rp.handle);
		}

	beginRenderPass(commandBuffer, finalRenderPass.handle, imageIndex, defaultScreenRect);
	vkCmdEndRenderPass( commandBuffer );
}

void VulkanApp::assignCallbacks()
{
	glfwSetCursorPosCallback(
		window_,
		[](GLFWwindow* window, double x, double y)
		{
			ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			void* ptr = glfwGetWindowUserPointer(window); 	
			const float mx = static_cast<float>(x / width);
			const float my = static_cast<float>(y / height);
			reinterpret_cast<VulkanApp*>(ptr)->handleMouseMove(mx, my);
		}
	);

	glfwSetMouseButtonCallback(
		window_,
		[](GLFWwindow* window, int button, int action, int mods)
		{
			auto& io = ImGui::GetIO();
			const int idx = button == GLFW_MOUSE_BUTTON_LEFT ? 0 : button == GLFW_MOUSE_BUTTON_RIGHT ? 2 : 1;
			io.MouseDown[idx] = action == GLFW_PRESS;

			void* ptr = glfwGetWindowUserPointer(window); 	
			reinterpret_cast<VulkanApp*>(ptr)->handleMouseClick(button, action == GLFW_PRESS);
		}
	);

	glfwSetKeyCallback(
		window_,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			const bool pressed = action != GLFW_RELEASE;
			if (key == GLFW_KEY_ESCAPE && pressed)
				glfwSetWindowShouldClose(window, GLFW_TRUE);

				void* ptr = glfwGetWindowUserPointer(window); 	
				reinterpret_cast<VulkanApp*>(ptr)->handleKey(key, pressed);
		}
	);
}

void VulkanApp::updateBuffers(uint32_t imageIndex)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)ctx_.vkDev.framebufferWidth, (float)ctx_.vkDev.framebufferHeight);
	ImGui::NewFrame();

	drawUI();

	ImGui::Render();

	draw3D();

	ctx_.updateBuffers(imageIndex);
}

void VulkanApp::mainLoop()
{
	double timeStamp = glfwGetTime();
	float deltaSeconds = 0.0f;

	do
	{
		update(deltaSeconds);

		const double newTimeStamp = glfwGetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;

		fpsCounter_.tick(deltaSeconds);

		bool frameRendered = drawFrame(ctx_.vkDev,
			[this](uint32_t img) { this->updateBuffers(img); },
			[this](auto cmd, auto img) { ctx_.composeFrame(cmd, img); }
		);

		fpsCounter_.tick(deltaSeconds, frameRendered);

		glfwPollEvents();

	} while (!glfwWindowShouldClose(window_));
}

void CameraApp::handleKey(int key, bool pressed)
{
	if (key == GLFW_KEY_W)
		positioner.movement_.forward_ = pressed;
	if (key == GLFW_KEY_S)
		positioner.movement_.backward_ = pressed;
	if (key == GLFW_KEY_A)
		positioner.movement_.left_ = pressed;
	if (key == GLFW_KEY_D)
		positioner.movement_.right_ = pressed;
	if (key == GLFW_KEY_C)
		positioner.movement_.up_ = pressed;
	if (key == GLFW_KEY_E)
		positioner.movement_.down_ = pressed;
}
