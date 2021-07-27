#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkFramework/GuiRenderer.h"
#include "shared/vkFramework/MultiRenderer.h"
#include "shared/vkFramework/QuadRenderer.h"
#include "shared/vkFramework/InfinitePlaneRenderer.h"

#include "ImGuizmo.h"

#include <math.h>

struct MyApp: public CameraApp
{
	MyApp()
	: CameraApp(-95, -95)
	, envMap(ctx_.resources.loadCubeMap("data/piazza_bologni_1k.hdr"))
	, irrMap(ctx_.resources.loadCubeMap("data/piazza_bologni_1k_irradiance.hdr"))
	, sceneData(ctx_, "data/meshes/test_graph.meshes", "data/meshes/test_graph.scene", "data/meshes/test_graph.materials", envMap, irrMap)
	, plane(ctx_)
	, multiRenderer(ctx_, sceneData)
	, imgui(ctx_)
	{
		onScreenRenderers_.emplace_back(plane, false);
		onScreenRenderers_.emplace_back(multiRenderer);
		onScreenRenderers_.emplace_back(imgui, false);

		sceneData.scene_.localTransform_[0] = glm::rotate(glm::mat4(1.f), (float)(M_PI / 2.f), glm::vec3(1.f, 0.f, 0.0f));
	}

	void drawUI() override {
		ImGui::Begin("Information", nullptr);
			ImGui::Text("FPS: %.2f", getFPS());
		ImGui::End();

		ImGui::Begin("Scene graph", nullptr);
			int node = renderSceneTree(sceneData.scene_, 0);
			if (node > -1)
				selectedNode = node;
		ImGui::End();

		editNode(selectedNode);
	}

	void draw3D() override {
		const mat4 p = getDefaultProjection();
		const mat4 view =camera.getViewMatrix();

		multiRenderer.setMatrices(p, view);
		plane.setMatrices(p, view, mat4(1.f));
	}

	void update(float deltaSeconds) override
	{
		CameraApp::update(deltaSeconds);

		// update/upload matrices for individual scene nodes
		sceneData.recalculateAllTransforms();
		sceneData.uploadGlobalTransforms();
	}

private:
	VulkanTexture envMap;
	VulkanTexture irrMap;

	VKSceneData sceneData;

	InfinitePlaneRenderer plane;
	MultiRenderer multiRenderer;
	GuiRenderer imgui;

	int selectedNode = -1;

	void editNode(int node)
	{
		if (node < 0)
			return;

		mat4 cameraProjection = getDefaultProjection();
		mat4 cameraView = camera.getViewMatrix();

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::BeginFrame();

		std::string name = getNodeName(sceneData.scene_, node);
		std::string label = name.empty() ? (std::string("Node") + std::to_string(node)) : name;
		label = "Node: " + label;

		ImGui::Begin("Editor");
			ImGui::Text("%s", label.c_str());

			glm::mat4 globalTransform = sceneData.scene_.globalTransform_[node]; // fetch global transform
			glm::mat4 srcTransform = globalTransform;
			glm::mat4 localTransform = sceneData.scene_.localTransform_[node];

			ImGui::Separator();
			ImGuizmo::SetID(1);

			editTransform(cameraView, cameraProjection, globalTransform);
			glm::mat4 deltaTransform = glm::inverse(srcTransform) * globalTransform;  // calculate delta for edited global transform
			sceneData.scene_.localTransform_[node] = localTransform * deltaTransform; // modify local transform
			markAsChanged(sceneData.scene_, node);

			ImGui::Separator();
			ImGui::Text("%s", "Material");

			editMaterial(node);
		ImGui::End();
	}

	void editTransform(const glm::mat4& view, const glm::mat4& projection, glm::mat4& matrix)
	{
		static ImGuizmo::OPERATION gizmoOperation(ImGuizmo::TRANSLATE);

		if (ImGui::RadioButton("Translate", gizmoOperation == ImGuizmo::TRANSLATE))
			gizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", gizmoOperation == ImGuizmo::ROTATE))
			gizmoOperation = ImGuizmo::ROTATE;

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), gizmoOperation, ImGuizmo::WORLD, glm::value_ptr(matrix));
	}

	void editMaterial(int node) {
		if (!sceneData.scene_.materialForNode_.contains(node))
			return;

		auto matIdx = sceneData.scene_.materialForNode_.at(node);
		MaterialDescription& material = sceneData.materials_[matIdx];

		float emissiveColor[4];
		memcpy(emissiveColor, &material.emissiveColor_, sizeof(gpuvec4));
		gpuvec4 oldColor = material.emissiveColor_;
		ImGui::ColorEdit3("Emissive color", emissiveColor);

		if (memcmp(emissiveColor, &oldColor, sizeof(gpuvec4))) {
			memcpy(&material.emissiveColor_, emissiveColor, sizeof(gpuvec4));
			sceneData.updateMaterial(matIdx);
		}
	}
};

int main()
{
	MyApp app;
	app.mainLoop();
	return 0;
}
