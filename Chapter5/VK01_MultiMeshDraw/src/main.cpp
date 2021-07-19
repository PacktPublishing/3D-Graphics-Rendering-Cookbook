#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkRenderers/VulkanClear.h"
#include "shared/vkRenderers/VulkanFinish.h"
#include "shared/vkRenderers/VulkanMultiMeshRenderer.h"

const uint32_t kScreenWidth = 1280;
const uint32_t kScreenHeight = 720;

GLFWwindow* window;

VulkanInstance vk;
VulkanRenderDevice vkDev;

std::unique_ptr<MultiMeshRenderer> multiRenderer;
std::unique_ptr<VulkanClear>   clear;
std::unique_ptr<VulkanFinish>  finish;

struct MouseState
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressedLeft = false;
} mouseState;

CameraPositioner_FirstPerson positioner_firstPerson(glm::vec3(0.0f, -5.0f, 15.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
Camera camera = Camera(positioner_firstPerson);

void initVulkan()
{
	createInstance(&vk.instance);

	if (!setupDebugCallbacks(vk.instance, &vk.messenger, &vk.reportCallback))
		exit(EXIT_FAILURE);

	if (glfwCreateWindowSurface(vk.instance, window, nullptr, &vk.surface))
		exit(EXIT_FAILURE);

	if (!initVulkanRenderDevice(vk, vkDev, kScreenWidth, kScreenHeight, isDeviceSuitable, { .multiDrawIndirect = VK_TRUE, .drawIndirectFirstInstance = VK_TRUE }))
		exit(EXIT_FAILURE);

	clear = std::make_unique<VulkanClear>(vkDev, VulkanImage());
	finish = std::make_unique<VulkanFinish>(vkDev, VulkanImage());

	multiRenderer = std::make_unique<MultiMeshRenderer>(vkDev,
		"data/meshes/test.meshes", "data/meshes/test.meshes.drawdata",
		"",
		"data/shaders/chapter05/VK01.vert", "data/shaders/chapter05/VK01.frag");
}

void terminateVulkan()
{
	finish = nullptr;
	clear = nullptr;

	multiRenderer = nullptr;

	destroyVulkanRenderDevice(vkDev);
	destroyVulkanInstance(vk);
}

void update3D(uint32_t imageIndex)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	const float ratio = width / (float)height;

	const mat4 m1 = glm::rotate(mat4(1.f), glm::pi<float>(), vec3(1, 0, 0));
	const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);

	const mat4 view = camera.getViewMatrix();
	const mat4 mtx = p * view * m1;

	multiRenderer->updateUniformBuffer(vkDev, imageIndex, mtx);
}

void composeFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	clear->fillCommandBuffer(commandBuffer, imageIndex);
	multiRenderer->fillCommandBuffer(commandBuffer, imageIndex);
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
			if (key == GLFW_KEY_W)
				positioner_firstPerson.movement_.forward_ = pressed;
			if (key == GLFW_KEY_S)
				positioner_firstPerson.movement_.backward_ = pressed;
			if (key == GLFW_KEY_A)
				positioner_firstPerson.movement_.left_ = pressed;
			if (key == GLFW_KEY_D)
				positioner_firstPerson.movement_.right_ = pressed;
			if (key == GLFW_KEY_C)
				positioner_firstPerson.movement_.up_ = pressed;
			if (key == GLFW_KEY_E)
				positioner_firstPerson.movement_.down_ = pressed;
			if (key == GLFW_KEY_SPACE)
				positioner_firstPerson.setUpVector(vec3(0.0f, 1.0f, 0.0f));
		}
	);

	initVulkan();

	double timeStamp = glfwGetTime();
	float deltaSeconds = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		positioner_firstPerson.update(deltaSeconds, mouseState.pos, mouseState.pressedLeft);

		const double newTimeStamp = glfwGetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;

		drawFrame(vkDev, &update3D, &composeFrame);

		glfwPollEvents();
	}

	terminateVulkan();

	glfwTerminate();

	glslang_finalize_process();

	return 0;
}
