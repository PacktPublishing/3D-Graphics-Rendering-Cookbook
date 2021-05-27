#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "shared/glFramework/GLFWApp.h"
#include "shared/glFramework/GLShader.h"
#include "shared/glFramework/GLSceneData.h"
#include "shared/glFramework/GLFramebuffer.h"
#include "shared/glFramework/UtilsGLImGui.h"
#include "shared/UtilsMath.h"
#include "shared/Camera.h"
#include "Chapter8/GLMesh8.h"

#include "shared/scene/VtxData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

struct PerFrameData
{
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
};

struct HDRParams
{
	float exposure_ = 0.9f;
	float maxWhite_ = 1.17f;
	float bloomStrength_ = 1.1f;
	float adaptationSpeed_ = 0.1f;
} g_HDRParams;

static_assert(sizeof(HDRParams) <= sizeof(PerFrameData));

struct MouseState
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressedLeft = false;
} mouseState;

CameraPositioner_FirstPerson positioner( vec3(-15.81f, 5.18f, -5.81f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
Camera camera(positioner);
bool g_EnableHDR = true;

int main(void)
{
	GLApp app;

	GLShader shdGridVert("data/shaders/chapter05/GL01_grid.vert");
	GLShader shdGridFrag("data/shaders/chapter05/GL01_grid.frag");
	GLProgram progGrid(shdGridVert, shdGridFrag);

	GLShader shdFullScreenQuadVert("data/shaders/chapter08/GL02_FullScreenQuad.vert");

	GLShader shdCombineHDR("data/shaders/chapter08/GL03_HDR.frag");
	GLProgram progCombineHDR(shdFullScreenQuadVert, shdCombineHDR);

	GLShader shdBlurX("data/shaders/chapter08/GL02_BlurX.frag");
	GLShader shdBlurY("data/shaders/chapter08/GL02_BlurY.frag");
	GLProgram progBlurX(shdFullScreenQuadVert, shdBlurX);
	GLProgram progBlurY(shdFullScreenQuadVert, shdBlurY);

	GLShader shdToLuminance("data/shaders/chapter08/GL03_ToLuminance.frag");
	GLProgram progToLuminance(shdFullScreenQuadVert, shdToLuminance);

	GLShader shdBrightPass("data/shaders/chapter08/GL03_BrightPass.frag");
	GLProgram progBrightPass(shdFullScreenQuadVert, shdBrightPass);

	GLShader shdAdaptation("data/shaders/chapter08/GL03_Adaptation.comp");
	GLProgram progAdaptation(shdAdaptation);

	GLShader shaderVertex("data/shaders/chapter08/GL03_scene_IBL.vert");
	GLShader shaderFragment("data/shaders/chapter08/GL03_scene_IBL.frag");
	GLProgram program(shaderVertex, shaderFragment);

	const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);

	GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, kBufferIndex_PerFrameUniforms, perFrameDataBuffer.getHandle(), 0, kUniformBufferSize);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	GLSceneData sceneData1("data/meshes/test.meshes", "data/meshes/test.scene", "data/meshes/test.materials");
	GLSceneData sceneData2("data/meshes/test2.meshes", "data/meshes/test2.scene", "data/meshes/test2.materials");

	GLMesh mesh1(sceneData1);
	GLMesh mesh2(sceneData2);

	glfwSetCursorPosCallback(
		app.getWindow(),
		[](auto* window, double x, double y)
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			mouseState.pos.x = static_cast<float>(x / width);
			mouseState.pos.y = static_cast<float>(y / height);
			ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
		}
	);

	glfwSetMouseButtonCallback(
		app.getWindow(),
		[](auto* window, int button, int action, int mods)
		{
			auto& io = ImGui::GetIO();
			const int idx = button == GLFW_MOUSE_BUTTON_LEFT ? 0 : button == GLFW_MOUSE_BUTTON_RIGHT ? 2 : 1;
			io.MouseDown[idx] = action == GLFW_PRESS;

			if (!io.WantCaptureMouse)
				if (button == GLFW_MOUSE_BUTTON_LEFT)
					mouseState.pressedLeft = action == GLFW_PRESS;
		}
	);

	glfwSetKeyCallback(
		app.getWindow(),
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
			if (key == GLFW_KEY_1)
				positioner.movement_.up_ = pressed;
			if (key == GLFW_KEY_2)
				positioner.movement_.down_ = pressed;
			if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
				positioner.movement_.fastSpeed_ = pressed;
			if (key == GLFW_KEY_SPACE)
				positioner.setUpVector(vec3(0.0f, 1.0f, 0.0f));
		}
	);

	positioner.maxSpeed_ = 1.0f;

	double timeStamp = glfwGetTime();
	float deltaSeconds = 0.0f;

	// offscreen render targets
	int width, height;
	glfwGetFramebufferSize(app.getWindow(), &width, &height);
	GLFramebuffer framebuffer(width, height, GL_RGBA16F, GL_DEPTH_COMPONENT24);
	GLFramebuffer luminance(64, 64, GL_RGBA16F, 0);
	GLFramebuffer brightPass(256, 256, GL_RGBA16F, 0);
	GLFramebuffer bloom1(256, 256, GL_RGBA16F, 0);
	GLFramebuffer bloom2(256, 256, GL_RGBA16F, 0);
	// create a texture view into the last mip-level (1x1 pixel) of our luminance framebuffer
	GLuint luminance1x1;
	glGenTextures(1, &luminance1x1);
	glTextureView(luminance1x1, GL_TEXTURE_2D, luminance.getTextureColor().getHandle(), GL_RGBA16F, 6, 1, 0, 1);
	// ping-pong textures for light adaptation
	GLTexture luminance1(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	GLTexture luminance2(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	const GLTexture* luminances[] = { &luminance1, &luminance2 };
	const vec4 brightPixel(vec3(50.0f), 1.0f);
	glTextureSubImage2D(luminance1.getHandle(), 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, glm::value_ptr(brightPixel));

	// cube map
	// https://hdrihaven.com/hdri/?h=immenstadter_horn
	GLTexture envMap(GL_TEXTURE_CUBE_MAP, "data/immenstadter_horn_2k.hdr");
	GLTexture envMapIrradiance(GL_TEXTURE_CUBE_MAP, "data/immenstadter_horn_2k_irradiance.hdr");
	GLShader shdCubeVertex("data/shaders/chapter08/GL03_cube.vert");
	GLShader shdCubeFragment("data/shaders/chapter08/GL03_cube.frag");
	GLProgram progCube(shdCubeVertex, shdCubeFragment);
	GLuint dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);
	const GLuint pbrTextures[] = { envMap.getHandle(), envMapIrradiance.getHandle() };
	glBindTextures(5, 2, pbrTextures);

	ImGuiGLRenderer rendererUI;

	while (!glfwWindowShouldClose(app.getWindow()))
	{
		positioner.update(deltaSeconds, mouseState.pos, mouseState.pressedLeft);

		const double newTimeStamp = glfwGetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;

		int width, height;
		glfwGetFramebufferSize(app.getWindow(), &width, &height);
		const float ratio = width / (float)height;

		glClearNamedFramebufferfv(framebuffer.getHandle(), GL_COLOR, 0, glm::value_ptr(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		glClearNamedFramebufferfi(framebuffer.getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);

		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
		const mat4 view = camera.getViewMatrix();

		const PerFrameData perFrameData = { .view = view, .proj = p, .cameraPos = glm::vec4(camera.getPosition(), 1.0f) };
		glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

		// 1. Render scene
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		framebuffer.bind();
		// 1.0 Cube map
		progCube.useProgram();
		glBindTextureUnit(1, envMap.getHandle());
		glDepthMask(false);
		glBindVertexArray(dummyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(true);
		// 1.1 Bistro
		program.useProgram();
		mesh1.draw(sceneData1);
		mesh2.draw(sceneData2);
		// 1.2 Grid
		glEnable(GL_BLEND);
		progGrid.useProgram();
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
		framebuffer.unbind();
		glGenerateTextureMipmap(framebuffer.getTextureColor().getHandle());
		glTextureParameteri(framebuffer.getTextureColor().getHandle(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		// pass HDR params to shaders
		glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, sizeof(g_HDRParams), &g_HDRParams);

		// 2.1 Downscale and convert to luminance
		luminance.bind();
		progToLuminance.useProgram();
		glBindTextureUnit(0, framebuffer.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		luminance.unbind();
		glGenerateTextureMipmap(luminance.getTextureColor().getHandle());

		// 2.2 Light adaptation
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		progAdaptation.useProgram();
#if 0
		// Either way is possible. In this case, all access modes will be GL_READ_WRITE
		const GLuint imageTextures[] = { luminances[0]->getHandle(), luminance1x1, luminances[1]->getHandle() };
		glBindImageTextures(0, 3, imageTextures);
#else
		glBindImageTexture(0, luminances[0]->getHandle(), 0, GL_TRUE, 0, GL_READ_ONLY,  GL_RGBA16F);
		glBindImageTexture(1, luminance1x1,					  0, GL_TRUE, 0, GL_READ_ONLY,  GL_RGBA16F);
		glBindImageTexture(2, luminances[1]->getHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
#endif
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

		// 2.3 Extract bright areas
		brightPass.bind();
		progBrightPass.useProgram();
		glBindTextureUnit(0, framebuffer.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		brightPass.unbind();
		glBlitNamedFramebuffer(brightPass.getHandle(), bloom2.getHandle(), 0, 0, 256, 256, 0, 0, 256, 256, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		for (int i = 0; i != 4; i++)
		{
			// 2.4 Blur X
			bloom1.bind();
			progBlurX.useProgram();
			glBindTextureUnit(0, bloom2.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 6);
			bloom1.unbind();
			// 2.5 Blur Y
			bloom2.bind();
			progBlurY.useProgram();
			glBindTextureUnit(0, bloom1.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 6);
			bloom2.unbind();
		}

		// 3. Apply tone mapping
		glViewport(0, 0, width, height);

		if (g_EnableHDR)
		{
			progCombineHDR.useProgram();
			glBindTextureUnit(0, framebuffer.getTextureColor().getHandle());
			glBindTextureUnit(1, luminances[1]->getHandle());
			glBindTextureUnit(2, bloom2.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else
		{
			glBlitNamedFramebuffer(framebuffer.getHandle(), 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)width, (float)height);
		ImGui::NewFrame();
		ImGui::Begin("Control", nullptr);
		ImGui::Checkbox("Enable HDR", &g_EnableHDR);
		// https://github.com/ocornut/imgui/issues/1889#issuecomment-398681105
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !g_EnableHDR);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * g_EnableHDR ? 1.0f : 0.2f);
		ImGui::Separator();
		ImGui::Text("Average luminance:");
		ImGui::Image((void*)(intptr_t)luminance1x1, ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::Text("Adapted luminance:");
		ImGui::Image((void*)(intptr_t)luminances[1]->getHandle(), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::Separator();
		ImGui::SliderFloat("Exposure", &g_HDRParams.exposure_, 0.1f, 2.0f);
		ImGui::SliderFloat("Max white", &g_HDRParams.maxWhite_, 0.5f, 2.0f);
		ImGui::SliderFloat("Bloom strength", &g_HDRParams.bloomStrength_, 0.0f, 2.0f);
		ImGui::SliderFloat("Adaptation speed", &g_HDRParams.adaptationSpeed_, 0.01f, 0.5f);
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
		ImGui::End();
		imguiTextureWindowGL("Color", framebuffer.getTextureColor().getHandle());
		imguiTextureWindowGL("Luminance", luminance.getTextureColor().getHandle());
		imguiTextureWindowGL("Bright Pass", brightPass.getTextureColor().getHandle());
		imguiTextureWindowGL("Bloom", bloom2.getTextureColor().getHandle());
		ImGui::Render();
		rendererUI.render(width, height, ImGui::GetDrawData());

		app.swapBuffers();

		// swap current and adapter luminances
		std::swap(luminances[0], luminances[1]);
	}

	glDeleteTextures(1, &luminance1x1);

	return 0;
}
