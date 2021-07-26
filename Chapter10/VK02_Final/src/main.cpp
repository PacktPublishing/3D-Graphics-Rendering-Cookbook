#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkFramework/GuiRenderer.h"
#include "shared/vkFramework/QuadRenderer.h"
#include "shared/vkFramework/CubeRenderer.h"
#include "shared/vkFramework/LineCanvas.h"

#include "shared/vkFramework/effects/SSAOProcessor.h"
#include "shared/vkFramework/effects/HDRProcessor.h"

#include "FinalRenderer.h"

const uint32_t TEX_RGB = (0x2 << 16);

#include <imgui/imgui_internal.h>

float g_LightPhi = -15.0f;
float g_LightTheta = +30.0f;

struct MyApp: public CameraApp
{
	MyApp()
	: CameraApp(-95, -95, {	.vertexPipelineStoresAndAtomics_ = true, .fragmentStoresAndAtomics_ = true })

	, colorTex(ctx_.resources.addColorTexture(0, 0, LuminosityFormat))
	, depthTex(ctx_.resources.addDepthTexture())
	, finalTex(ctx_.resources.addColorTexture(0, 0, LuminosityFormat))

	, luminanceResult(ctx_.resources.addColorTexture(1, 1, LuminosityFormat))

	, envMap(ctx_.resources.loadCubeMap("data/immenstadter_horn_2k.hdr"))
	, irrMap(ctx_.resources.loadCubeMap("data/immenstadter_horn_2k_irradiance.hdr"))
	, sceneData(ctx_, "data/meshes/bistro_all.meshes", "data/meshes/bistro_all.scene", "data/meshes/bistro_all.materials", envMap, irrMap, true)
	, cubeRenderer(ctx_, envMap, { colorTex, depthTex },
			ctx_.resources.addRenderPass({ colorTex, depthTex }, RenderPassCreateInfo {
			.clearColor_ = true, .clearDepth_ = true, .flags_ = eRenderPassBit_First | eRenderPassBit_Offscreen }))

	// Renderer with opaque/transparent object management and OIT composition
	, finalRenderer(ctx_, sceneData, { colorTex, depthTex })

	// tone mapping (gamma correction / exposure)
	, luminance(ctx_, finalTex, luminanceResult)
	, hdrUniformBuffer(mappedUniformBufferAttachment(ctx_.resources, &hdrUniforms, VK_SHADER_STAGE_FRAGMENT_BIT))
	, hdr(ctx_, finalTex, luminanceResult, hdrUniformBuffer)

	, ssao(ctx_, finalRenderer.outputColor /*colorTex for no-HDR */, depthTex, finalTex)

	, displayedTextureList({
				finalRenderer.shadowDepth, finalTex, depthTex, ssao.getBlurY(),              // 0 - 3
				colorTex, luminance.getResult64(),                                           // 4 - 5
				luminance.getResult32(), luminance.getResult16(), luminance.getResult08(),   // 6 - 8
				luminance.getResult04(), luminance.getResult02(), luminance.getResult01(),   // 9 - 11
				hdr.getBloom1(), hdr.getBloom2(), hdr.getBrightness(), hdr.getResult(),      // 12 - 15
				hdr.getStreaks1(), hdr.getStreaks2(),                                        // 16 - 17
				hdr.getAdaptatedLum1(), hdr.getAdaptatedLum2(),                              // 18 - 19
				finalRenderer.outputColor                                                    // 20
		})

	, quads(ctx_, displayedTextureList)
	, imgui(ctx_, displayedTextureList)

	, canvas(ctx_)

	, lumToColor(ctx_, colorTex)
	, toDepth(ctx_, depthTex)
	, lumWait(ctx_, colorTex)
	{
		positioner = CameraPositioner_FirstPerson(glm::vec3(-10.0f, -3.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

		hdrUniforms->bloomStrength = 1.1f;
		hdrUniforms->maxWhite = 1.17f;
		hdrUniforms->exposure = 0.9f;
		hdrUniforms->adaptationSpeed = 0.1f;

		setVkImageName(ctx_.vkDev, colorTex.image.image, "color");
		setVkImageName(ctx_.vkDev, depthTex.image.image, "depth");
		setVkImageName(ctx_.vkDev, finalTex.image.image, "final");

		setVkImageName(ctx_.vkDev, finalRenderer.shadowColor.image.image, "shadowColor");
		setVkImageName(ctx_.vkDev, finalRenderer.shadowDepth.image.image, "shadowDepth");

		onScreenRenderers_.emplace_back(cubeRenderer);        // 0
		onScreenRenderers_.emplace_back(toDepth, false);      // 1
		onScreenRenderers_.emplace_back(lumToColor, false);   // 2

		onScreenRenderers_.emplace_back(finalRenderer);       // 3

		onScreenRenderers_.emplace_back(ssao);                // 4

		onScreenRenderers_.emplace_back(lumWait, false);      // 5
		onScreenRenderers_.emplace_back(luminance, false);    // 6
		onScreenRenderers_.emplace_back(hdr, false);          // 7

		onScreenRenderers_.emplace_back(quads, false);        // 8
		onScreenRenderers_.emplace_back(imgui, false);        // 9

		onScreenRenderers_.emplace_back(canvas);              // 10

		{
			std::vector<BoundingBox> reorderedBoxes;
			reorderedBoxes.reserve(sceneData.shapes_.size());

			// pretransform bounding boxes to world space
			for (const auto& c : sceneData.shapes_)
			{
				const mat4 model = sceneData.scene_.globalTransform_[c.transformIndex];
				reorderedBoxes.push_back(sceneData.meshData_.boxes_[c.meshIndex]);
				reorderedBoxes.back().transform(model);
			}

			bigBox = reorderedBoxes.front();
			for (const auto& b : reorderedBoxes)
			{
				bigBox.combinePoint(b.min_);
				bigBox.combinePoint(b.max_);
			}
		}
	}

	void drawUI() override {
		const float indentSize = 16.0f;

		ImGui::Begin("Control", nullptr);

		ImGui::Checkbox("Show object bounding boxes", &showObjectBoxes);
		ImGui::Checkbox("Render transparent objects", &finalRenderer.renderTransparentObjects);

		ImGui::Text("HDR");
		ImGui::Indent(indentSize);

			ImGui::Checkbox("Show Pyramid", &showPyramid);
			ImGui::Checkbox("Show HDR Debug", &showHDRDebug);

			ImGui::SliderFloat("Exposure", &hdrUniforms->exposure, 0.1f, 2.0f);
			ImGui::SliderFloat("Max white", &hdrUniforms->maxWhite, 0.5f, 2.0f);
			ImGui::SliderFloat("Bloom strength", &hdrUniforms->bloomStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("Adaptation speed", &hdrUniforms->adaptationSpeed, 0.01f, 0.5f);

		ImGui::Unindent(indentSize);
		ImGui::Separator();

		ImGui::Text("SSAO");
		ImGui::Indent(indentSize);
			ImGui::Checkbox("Show SSAO buffer", &showSSAODebug);

			ImGui::SliderFloat("SSAO scale", &ssao.params->scale_, 0.0f, 2.0f);
			ImGui::SliderFloat("SSAO bias",  &ssao.params->bias_, 0.0f, 0.3f);
		ImGui::Separator();
			ImGui::SliderFloat("SSAO radius", &ssao.params->radius, 0.05f, 0.5f);
			ImGui::SliderFloat("SSAO attenuation scale", &ssao.params->attScale, 0.5f, 1.5f);
			ImGui::SliderFloat("SSAO distance scale", &ssao.params->distScale, 0.0f, 1.0f);

		ImGui::Unindent(indentSize);
		ImGui::Separator();

		ImGui::Text("Shadows:");
		ImGui::Indent(indentSize);
		ImGui::Checkbox("Enable shadows", &finalRenderer.enableShadows);

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !finalRenderer.enableShadows);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * finalRenderer.enableShadows ? 1.0f : 0.2f);

				ImGui::Checkbox("Show shadow buffer", &showShadowBuffer);
				ImGui::Checkbox("Show light frustum", &showLightFrustum);

				ImGui::SliderFloat("Light Theta", &g_LightTheta, -85.0f, +85.0f);
				ImGui::SliderFloat("Light Phi", &g_LightPhi, -85.0f, +85.0f);

			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		ImGui::Unindent(indentSize);

		ImGui::End();

		if (showPyramid)
		{
			ImGui::Begin("Pyramid", nullptr);

			ImGui::Text("HDRColor");
			ImGui::Image((void*)(intptr_t)(5 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Lum64");
			ImGui::Image((void*)(intptr_t)(6 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Lum32");
			ImGui::Image((void*)(intptr_t)(7 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Lum16");
			ImGui::Image((void*)(intptr_t)(8 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Lum08");
			ImGui::Image((void*)(intptr_t)(9 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Lum04");
			ImGui::Image((void*)(intptr_t)(10| TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Lum02");
			ImGui::Image((void*)(intptr_t)(11| TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Lum01");
			ImGui::Image((void*)(intptr_t)(12| TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::End();
		}

		if (showHDRDebug)
		{
			ImGui::Begin("Adaptation", nullptr);
			ImGui::Text("Adapt1");
			ImGui::Image((void*)(intptr_t)(19 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Adapt2");
			ImGui::Image((void*)(intptr_t)(20 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::End();

			ImGui::Begin("Debug", nullptr);
			ImGui::Text("Bloom1");
			ImGui::Image((void*)(intptr_t)(13 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Bloom2");
			ImGui::Image((void*)(intptr_t)(14 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Bright");
			ImGui::Image((void*)(intptr_t)(15 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Result");
			ImGui::Image((void*)(intptr_t)(16 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

			ImGui::Text("Streaks1");
			ImGui::Image((void*)(intptr_t)(17 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::Text("Streaks2");
			ImGui::Image((void*)(intptr_t)(18 | TEX_RGB), ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::End();
		}

		if (finalRenderer.enableShadows && showShadowBuffer)
			imguiTextureWindow("Shadow", 1 | (1 << 16));

		if (enableSSAO && showSSAODebug)
			imguiTextureWindow("SSAO", 4);
	}

	bool showPyramid = false;
	bool showHDRDebug = false;

	bool enableHDR = true;

	bool enableSSAO = true;
	bool showSSAODebug = false;
	bool showShadowBuffer = false;

	bool showLightFrustum = false;
	bool showObjectBoxes = false;

	void draw3D() override {
		const mat4 p = getDefaultProjection();
		const mat4 view =camera.getViewMatrix();

		canvas.setCameraMatrix(p * view);
		canvas.clear();

		const glm::mat4 rot1 = glm::rotate(mat4(1.f), glm::radians(g_LightTheta), glm::vec3(0, 0, 1));
		const glm::mat4 rot2 = glm::rotate(rot1, glm::radians(g_LightPhi), glm::vec3(1, 0, 0));
		vec3 lightDir = glm::normalize(vec3(rot2 * vec4(0.0f, -1.0f, 0.0f, 1.0f)));
		const mat4 lightView = glm::lookAt(glm::vec3(0.0f), lightDir, vec3(0, 0, 1));

		const BoundingBox box = bigBox.getTransformed(lightView);
		const mat4 lightProj = finalRenderer.enableShadows ? glm::ortho(box.min_.x, box.max_.x, box.min_.y, box.max_.y, -box.max_.z, -box.min_.z) : mat4(0.f);

		if (finalRenderer.enableShadows && showLightFrustum)
		{
			drawBox3d(canvas, glm::scale(glm::mat4(1.f), vec3(1, -1, 1)), bigBox, glm::vec4(0, 0, 0, 1));
			drawBox3d(canvas, glm::mat4(1.f), box, vec4(1, 0, 0, 1));
			renderCameraFrustum(canvas, lightView, lightProj, vec4(1.0f, 0.0f, 0.0f, 1.0f));

			canvas.line(vec3(0.0f), lightDir * 100.0f, vec4(0, 0, 1, 1));
		}

		if (showObjectBoxes)
			for (const auto& b : sceneData.meshData_.boxes_)
				drawBox3d(canvas, glm::scale(glm::mat4(1.f), vec3(1, -1, 1)), b, glm::vec4(0, 1, 0, 1));

		cubeRenderer.setMatrices(p, view);

		finalRenderer.setMatrices(p, view);
		finalRenderer.setLightParameters(lightProj, lightView);
		finalRenderer.setCameraPosition(positioner.getPosition());

		for (int i = 0 ; i < 25 ; i++)
			finalRenderer.checkLoadedTextures();

		quads.clear();
		quads.quad(-1.0f, enableHDR ? 1.0f : -1.0f, 1.0f, enableHDR ? -1.0f : 1.0f, 15);
	}
private:
	HDRUniformBuffer* hdrUniforms;

	VulkanTexture colorTex, depthTex, finalTex;

	VulkanTexture luminanceResult;

	VulkanTexture envMap;
	VulkanTexture irrMap;

	VKSceneData sceneData;

	CubemapRenderer cubeRenderer;
	FinalMultiRenderer finalRenderer;

	LuminanceCalculator luminance;

	BufferAttachment hdrUniformBuffer;
	HDRProcessor hdr;

	SSAOProcessor ssao;

	std::vector<VulkanTexture> displayedTextureList;

	QuadRenderer quads;

	GuiRenderer imgui;
	LineCanvas canvas;

	BoundingBox bigBox;

	ShaderOptimalToDepthBarrier toDepth;
	ShaderOptimalToColorBarrier lumToColor;

	ColorWaitBarrier lumWait;
};

int main()
{
	MyApp app;
	app.mainLoop();
	return 0;
}
