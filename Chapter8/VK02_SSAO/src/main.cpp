#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkFramework/GuiRenderer.h"
#include "shared/vkFramework/MultiRenderer.h"
#include "shared/vkFramework/QuadRenderer.h"

const char* envMapFile = "data/piazza_bologni_1k.hdr";
const char* irrMapFile = "data/piazza_bologni_1k_irradiance.hdr";

#include "shared/vkFramework/effects/SSAOProcessor.h"

#include <imgui/imgui_internal.h>

struct MyApp: public CameraApp
{
	MyApp(): CameraApp(-95, -95),
		colorTex(ctx_.resources.addColorTexture()),
		depthTex(ctx_.resources.addDepthTexture()),
		finalTex(ctx_.resources.addColorTexture()),

		sceneData(ctx_, "data/meshes/test.meshes", "data/meshes/test.scene", "data/meshes/test.materials", 
				ctx_.resources.loadCubeMap(envMapFile),
				ctx_.resources.loadCubeMap(irrMapFile)),

		multiRenderer(ctx_, sceneData, "data/shaders/chapter07/VK01.vert", "data/shaders/chapter07/VK01.frag", { colorTex, depthTex }, 
			ctx_.resources.addRenderPass({ colorTex, depthTex }, RenderPassCreateInfo {
			.clearColor_ = true, .clearDepth_ = true, .flags_ = eRenderPassBit_First | eRenderPassBit_Offscreen })),

		SSAO(ctx_, colorTex, depthTex, finalTex),

		quads(ctx_, { colorTex, finalTex }),
		imgui(ctx_, { colorTex, SSAO.getBlurY() })
	{
		positioner = CameraPositioner_FirstPerson(glm::vec3(-10.0f, -3.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

		onScreenRenderers_.emplace_back(multiRenderer);
		onScreenRenderers_.emplace_back(SSAO);

		onScreenRenderers_.emplace_back(quads, false);
		onScreenRenderers_.emplace_back(imgui, false);
	}

	bool enableSSAO = true;

	void drawUI() override {
		ImGui::Begin("Control", nullptr);
			ImGui::Checkbox("Enable SSAO", &enableSSAO);
			// https://github.com/ocornut/imgui/issues/1889#issuecomment-398681105
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !enableSSAO);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * enableSSAO ? 1.0f : 0.2f);

			ImGui::SliderFloat("SSAO scale", &SSAO.params->scale_, 0.0f, 2.0f);
			ImGui::SliderFloat("SSAO bias",  &SSAO.params->bias_, 0.0f, 0.3f);
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		ImGui::Separator();
			ImGui::SliderFloat("SSAO radius", &SSAO.params->radius, 0.05f, 0.5f);
			ImGui::SliderFloat("SSAO attenuation scale", &SSAO.params->attScale, 0.5f, 1.5f);
			ImGui::SliderFloat("SSAO distance scale", &SSAO.params->distScale, 0.0f, 1.0f);
		ImGui::End();


		if (enableSSAO) {
			imguiTextureWindow("SSAO", 2);
		}
	}

	void draw3D() override {
		multiRenderer.setMatrices(getDefaultProjection(), camera.getViewMatrix());

		quads.clear();
		quads.quad(-1.0f, -1.0f, 1.0f, 1.0f, enableSSAO ? 1 : 0);
	}

private:
	VulkanTexture colorTex, depthTex, finalTex;

	VKSceneData sceneData;

	MultiRenderer multiRenderer;

	SSAOProcessor SSAO;

	QuadRenderer quads;

	GuiRenderer imgui;
};

int main()
{
	MyApp app;
	app.mainLoop();
	return 0;
}
