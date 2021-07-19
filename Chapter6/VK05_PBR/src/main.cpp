#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkRenderers/VulkanClear.h"
#include "shared/vkRenderers/VulkanFinish.h"
#include "shared/vkRenderers/VulkanPBRModelRenderer.h"

#include "shared/Camera.h"

std::unique_ptr<PBRModelRenderer> modelPBR;

glm::vec3 cameraPos(0.0f, 0.0f, 0.0f);
glm::vec3 cameraAngles(-45.0f, 0.0f, 0.0f);

CameraPositioner_FirstPerson positioner(vec3(0.0f, 6.0f, 11.0f), vec3(0.0f, 4.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
Camera camera = Camera(positioner);

struct MouseState
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressedLeft = false;
} mouseState;

GLFWwindow* window;

const uint32_t kScreenWidth = 1600;
const uint32_t kScreenHeight = 900;

VulkanInstance vk;
VulkanRenderDevice vkDev;

std::unique_ptr<VulkanClear> clear;
std::unique_ptr<VulkanFinish> finish;

struct UBO
{
	mat4 mvp, mv, m;
	vec4 cameraPos;
} ubo;

void updateBuffers(uint32_t imageIndex)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	const float ratio = width / (float)height;

	const mat4 scale = glm::scale(mat4(1.0f), vec3(5.0f));
	const mat4 rot1 = glm::rotate(mat4(1.0f), glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	const mat4 rot2 = glm::rotate(mat4(1.0f), glm::radians(180.0f), vec3(0.0f, 0.0f, 1.0f));
	const mat4 rot = rot1 * rot2;
	const mat4 pos = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, +1.0f));
	const mat4 m = glm::rotate(scale * rot * pos, (float)glfwGetTime() * -0.1f, vec3(0.0f, 0.0f, 1.0f));
	const mat4 proj = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
	const mat4 mvp = proj * camera.getViewMatrix() * m;

	const mat4 mv = camera.getViewMatrix() * m;

	ubo = UBO { .mvp = mvp, .mv = mv, .m = m, .cameraPos = vec4(positioner.getPosition(), 1.0f) };
	modelPBR->updateUniformBuffer(vkDev, imageIndex, &ubo, sizeof(ubo));
}

void composeFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	clear->fillCommandBuffer(commandBuffer, imageIndex);
	modelPBR->fillCommandBuffer(commandBuffer, imageIndex);
	finish->fillCommandBuffer(commandBuffer, imageIndex);
}

int main()
{
	window = initVulkanApp(kScreenWidth, kScreenHeight);

	glfwSetMouseButtonCallback(
		window,
		[](auto* window, int button, int action, int mods)
		{
			const int idx = button == GLFW_MOUSE_BUTTON_LEFT ? 0 : button == GLFW_MOUSE_BUTTON_RIGHT ? 2 : 1;
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				mouseState.pressedLeft = action == GLFW_PRESS;
		}
	);

	glfwSetKeyCallback(
		window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			const bool pressed = action != GLFW_RELEASE;
			if (key == GLFW_KEY_ESCAPE && pressed)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			if (key == GLFW_KEY_W)
				positioner.movement_.forward_ = pressed;
			if (key == GLFW_KEY_S)
				positioner.movement_.backward_ = pressed;
			if (key == GLFW_KEY_A)
				positioner.movement_.left_ = pressed;
			if (key == GLFW_KEY_D)
				positioner.movement_.right_ = pressed;
			if (key == GLFW_KEY_SPACE)
				positioner.setUpVector(vec3(0.0f, 1.0f, 0.0f));
		});

	createInstance(&vk.instance);

	BL_CHECK(setupDebugCallbacks(vk.instance, &vk.messenger, &vk.reportCallback));
	VK_CHECK(glfwCreateWindowSurface(vk.instance, window, nullptr, &vk.surface));
	BL_CHECK(initVulkanRenderDeviceWithCompute(vk, vkDev, kScreenWidth, kScreenHeight, VkPhysicalDeviceFeatures{}));

	VulkanImage depthTexture;
	createDepthResources(vkDev, vkDev.framebufferWidth, vkDev.framebufferHeight, depthTexture);

	clear = std::make_unique<VulkanClear>(vkDev, depthTexture);
	finish = std::make_unique<VulkanFinish>(vkDev, depthTexture);

	modelPBR = std::make_unique<PBRModelRenderer>(vkDev,
			(uint32_t)sizeof(UBO),
			"deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf",
			"deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_AO.jpg",
			"deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_emissive.jpg",
			"deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_albedo.jpg",
			"deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_metalRoughness.jpg",
			"deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_normal.jpg",
			"data/piazza_bologni_1k.hdr",
			"data/piazza_bologni_1k_irradiance.hdr",
			depthTexture);

	double lastTime = glfwGetTime();

	do
	{
		drawFrame(vkDev, updateBuffers, composeFrame);

		const double newTime = glfwGetTime();
		const float deltaSeconds = static_cast<float>(newTime - lastTime);
		positioner.update(deltaSeconds, mouseState.pos, mouseState.pressedLeft);
		lastTime = newTime;

		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	clear = nullptr;
	finish = nullptr;

	modelPBR = nullptr;

	destroyVulkanImage(vkDev.device, depthTexture);

	destroyVulkanRenderDevice(vkDev);
	destroyVulkanInstance(vk);

	glfwTerminate();
	glslang_finalize_process();

	return 0;
}
