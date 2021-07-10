#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "shared/glFramework/GLFWApp.h"
#include "shared/glFramework/GLShader.h"
#include "shared/glFramework/GLSceneData.h"
#include "shared/glFramework/GLFramebuffer.h"
#include "shared/glFramework/LineCanvasGL.h"
#include "shared/glFramework/UtilsGLImGui.h"
#include "shared/UtilsMath.h"
#include "shared/Camera.h"
#include "shared/scene/VtxData.h"
#include "Chapter9/GLMesh9.h"
#include "Chapter10/GLSkyboxRenderer.h"

struct PerFrameData
{
	mat4 view;
	mat4 proj;
	mat4 light = mat4(0.0f); // unused in this demo
	vec4 cameraPos;
};

struct MouseState
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressedLeft = false;
} mouseState;

CameraPositioner_FirstPerson positioner( vec3(-10.0f, 3.0f, 3.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
Camera camera(positioner);

mat4 g_CullingView = camera.getViewMatrix();
bool g_FreezeCullingView = false;
bool g_DrawMeshes = true;
bool g_DrawBoxes = true;
bool g_DrawGrid = true;

int main(void)
{
	GLApp app;

	GLShader shdGridVert("data/shaders/chapter05/GL01_grid.vert");
	GLShader shdGridFrag("data/shaders/chapter05/GL01_grid.frag");
	GLProgram progGrid(shdGridVert, shdGridFrag);
	GLShader shaderVert("data/shaders/chapter10/GL01_scene_IBL.vert");
	GLShader shaderFrag("data/shaders/chapter10/GL01_scene_IBL.frag");
	GLProgram program(shaderVert, shaderFrag);

	const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);

	GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, kBufferIndex_PerFrameUniforms, perFrameDataBuffer.getHandle(), 0, kUniformBufferSize);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	GLSceneData sceneData("data/meshes/bistro_all.meshes", "data/meshes/bistro_all.scene", "data/meshes/bistro_all.materials");
	GLMesh mesh(sceneData);

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

			if (key == GLFW_KEY_P && pressed)
				g_FreezeCullingView = !g_FreezeCullingView;
		}
	);

	positioner.maxSpeed_ = 1.0f;

	GLSkyboxRenderer skybox;
	ImGuiGLRenderer rendererUI;
	CanvasGL canvas;

	// pretransform bounding boxes to world space
	for (const auto& c : sceneData.shapes_)
	{
		const mat4 model = sceneData.scene_.globalTransform_[c.transformIndex];
		sceneData.meshData_.boxes_[c.meshIndex].transform(model);
	}

	const BoundingBox fullScene = combineBoxes(sceneData.meshData_.boxes_);

	while (!glfwWindowShouldClose(app.getWindow()))
	{
		positioner.update(app.getDeltaSeconds(), mouseState.pos, mouseState.pressedLeft);

		int width, height;
		glfwGetFramebufferSize(app.getWindow(), &width, &height);
		const float ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClearNamedFramebufferfv(0, GL_COLOR, 0, glm::value_ptr(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		glClearNamedFramebufferfi(0, GL_DEPTH_STENCIL, 0, 1.0f, 0);

		const mat4 proj = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
		const mat4 view = camera.getViewMatrix();

		const PerFrameData perFrameData = { .view = view, .proj = proj, .light = mat4(0.0f), .cameraPos = glm::vec4(camera.getPosition(), 1.0f) };
		glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

		if (!g_FreezeCullingView)
			g_CullingView = camera.getViewMatrix();

		vec4 frustumPlanes[6];
		getFrustumPlanes(proj * g_CullingView, frustumPlanes);
		vec4 frustumCorners[8];
		getFrustumCorners(proj * g_CullingView, frustumCorners);

		// cull
		int numVisibleMeshes = 0;
		{
			DrawElementsIndirectCommand* cmd = mesh.bufferIndirect_.drawCommands_.data();
			for (const auto& c : sceneData.shapes_)
			{
				cmd->instanceCount_ = isBoxInFrustum(frustumPlanes, frustumCorners, sceneData.meshData_.boxes_[c.meshIndex]) ? 1 : 0;
				numVisibleMeshes += (cmd++)->instanceCount_;
			}
			mesh.bufferIndirect_.uploadIndirectBuffer();
		}

		if (g_DrawBoxes)
		{
			DrawElementsIndirectCommand* cmd = mesh.bufferIndirect_.drawCommands_.data();
			for (const auto& c : sceneData.shapes_)
				drawBox3dGL(canvas, mat4(1.0f), sceneData.meshData_.boxes_[c.meshIndex], (cmd++)->instanceCount_ ? vec4(0, 1, 0, 1) : vec4(1, 0, 0, 1));
			drawBox3dGL(canvas, mat4(1.0f), fullScene, vec4(1, 0, 0, 1));
		}

		// 1. Render scene
		skybox.draw();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		// 1.1 Bistro
		if (g_DrawMeshes)
		{
			program.useProgram();
			mesh.draw(sceneData.shapes_.size());
		}

		// 1.2 Grid
		glEnable(GL_BLEND);

		if (g_DrawGrid)
		{
			progGrid.useProgram();
			glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
		}

		if (g_FreezeCullingView)
			renderCameraFrustumGL(canvas, g_CullingView, proj, vec4(1, 1, 0,1), 100);
		canvas.flush();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)width, (float)height);
		ImGui::NewFrame();
		ImGui::Begin("Control", nullptr);
		ImGui::Text("Draw:");
		ImGui::Checkbox("Meshes", &g_DrawMeshes);
		ImGui::Checkbox("Boxes",  &g_DrawBoxes);
		ImGui::Checkbox("Grid",  &g_DrawGrid);
		ImGui::Separator();
		ImGui::Checkbox("Freeze culling frustum (P)", &g_FreezeCullingView);
		ImGui::Separator();
		ImGui::Text("Visible meshes: %i", numVisibleMeshes);
		ImGui::End();
		ImGui::Render();
		rendererUI.render(width, height, ImGui::GetDrawData());

		app.swapBuffers();
	}

	return 0;
}
